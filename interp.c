/* FILE : interp.c */

/*
07-May-03 condnestrec
17-Mar-03 modules
04-Dec-02 argc, argv
04-Nov-02 undefs
03-Apr-02 # comments
01-Feb-02 opcase

30-Oct-01 Fixed Bugs in file interp.c :

   1. division (/) by zero when one or both operands are floats
   2. fremove and frename gave wrong truth value
      (now success  => true)
   3. nullary, unary, binary, ternary had two bugs:
      a. coredump when there was nothing to push
	 e.g.   11 22 [pop pop] nullary
      b. produced circular ("infinite") list when no
	 new node had been created
	 e.g.   11 22 [pop] nullary
   4. app4 combinator was wrong.
      Also renamed:  app2 -> unary2,  app3 -> unary3, app4 -> unary4
      the old names can still be used, but are declared obsolete.
   5. Small additions to (raw) Joy:
      a)  putchars - previously defined in library file inilib.joy
      b) fputchars (analogous, for specified file)
	 fputstring (== fputchars for Heiko Kuhrt's program)
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "globals.h"
# ifdef GC_BDW
#    include <gc.h>
#    define malloc GC_malloc_atomic
#    define realloc GC_realloc
#    define free(X)
# endif

PRIVATE void helpdetail_();		/* this file		*/
PRIVATE void undefs_();
PRIVATE void make_manual(int style /* 0=plain, 1=html, 2=latex */);
PRIVATE void manual_list_();
PRIVATE void manual_list_aux_();

#define ONEPARAM(NAME)						\
    if (stk == NULL)						\
	execerror("one parameter",NAME)
#define TWOPARAMS(NAME)						\
    if (stk == NULL || stk->next == NULL)			\
	execerror("two parameters",NAME)
#define THREEPARAMS(NAME)					\
    if (stk == NULL || stk->next == NULL			\
	    || stk->next->next == NULL)				\
	execerror("three parameters",NAME)
#define FOURPARAMS(NAME)					\
    if (stk == NULL || stk->next == NULL			\
	    || stk->next->next == NULL				\
	    || stk->next->next->next == NULL)			\
	execerror("four parameters",NAME)
#define FIVEPARAMS(NAME)					\
    if (stk == NULL || stk->next == NULL			\
	    || stk->next->next == NULL				\
	    || stk->next->next->next == NULL			\
	    || stk->next->next->next->next == NULL)		\
	execerror("four parameters",NAME)
#define ONEQUOTE(NAME)						\
    if (stk->op != LIST_)					\
	execerror("quotation as top parameter",NAME)
#define TWOQUOTES(NAME)						\
    ONEQUOTE(NAME);						\
    if (stk->next->op != LIST_)					\
	execerror("quotation as second parameter",NAME)
#define THREEQUOTES(NAME)					\
    TWOQUOTES(NAME);						\
    if (stk->next->next->op != LIST_)				\
	execerror("quotation as third parameter",NAME)
#define FOURQUOTES(NAME)					\
    THREEQUOTES(NAME);						\
    if (stk->next->next->next->op != LIST_)			\
	execerror("quotation as fourth parameter",NAME)
#define SAME2TYPES(NAME)					\
    if (stk->op != stk->next->op)				\
	execerror("two parameters of the same type",NAME)
#define STRING(NAME)						\
    if (stk->op != STRING_)					\
	execerror("string",NAME)
#define STRING2(NAME)						\
    if (stk->next->op != STRING_)				\
	execerror("string as second parameter",NAME)
#define INTEGER(NAME)						\
    if (stk->op != INTEGER_)					\
	execerror("integer",NAME)
#define INTEGER2(NAME)						\
    if (stk->next->op != INTEGER_)				\
	execerror("integer as second parameter",NAME)
#define CHARACTER(NAME)						\
    if (stk->op != CHAR_)					\
	execerror("character",NAME)
#define INTEGERS2(NAME)						\
    if (stk->op != INTEGER_ || stk->next->op != INTEGER_)	\
	execerror("two integers",NAME)
#define NUMERICTYPE(NAME)					\
    if (stk->op != INTEGER_ && stk->op !=  CHAR_		\
	  && stk->op != BOOLEAN_ )				\
	execerror("numeric",NAME)
#define NUMERIC2(NAME)						\
    if (stk->next->op != INTEGER_ && stk->next->op != CHAR_)	\
	execerror("numeric second parameter",NAME)
#define FLOATABLE						\
    (stk->op == INTEGER_ || stk->op == FLOAT_)
#define FLOATABLE2						\
    ((stk->op == FLOAT_ && stk->next->op == FLOAT_) ||		\
	(stk->op == FLOAT_ && stk->next->op == INTEGER_) ||	\
	(stk->op == INTEGER_ && stk->next->op == FLOAT_))
#define FLOAT(NAME)						\
    if (!FLOATABLE)						\
	execerror("float or integer", NAME);
#define FLOAT2(NAME)						\
    if (!(FLOATABLE2 || (stk->op == INTEGER_ && stk->next->op == INTEGER_))) \
	execerror("two floats or integers", NAME)
#define FLOATVAL						\
    (stk->op == FLOAT_ ? stk->u.dbl : (double) stk->u.num)
#define FLOATVAL2						\
    (stk->next->op == FLOAT_ ? stk->next->u.dbl : (double) stk->next->u.num)
#define FLOAT_U(OPER)						\
    if (FLOATABLE) { UNARY(FLOAT_NEWNODE, OPER(FLOATVAL)); return; }
#define FLOAT_P(OPER)						\
    if (FLOATABLE2) { BINARY(FLOAT_NEWNODE, OPER(FLOATVAL2, FLOATVAL)); return; }
#define FLOAT_I(OPER)						\
    if (FLOATABLE2) { BINARY(FLOAT_NEWNODE, (FLOATVAL2) OPER (FLOATVAL)); return; }
#define FILE(NAME)						\
    if (stk->op != FILE_ || stk->u.fil == NULL)			\
	execerror("file", NAME)
#define CHECKZERO(NAME)						\
    if (stk->u.num == 0)					\
	execerror("non-zero operand",NAME)
#define LIST(NAME)						\
    if (stk->op != LIST_)					\
	execerror("list",NAME)
#define LIST2(NAME)						\
    if (stk->next->op != LIST_)					\
	execerror("list as second parameter",NAME)
#define USERDEF(NAME)						\
    if (stk->op != USR_)					\
	execerror("user defined symbol",NAME)
#define CHECKLIST(OPR,NAME)					\
    if (OPR != LIST_)						\
	execerror("internal list",NAME)
#define CHECKSETMEMBER(NODE,NAME)				\
    if ((NODE->op != INTEGER_ && NODE->op != CHAR_) ||		\
	NODE->u.num >= SETSIZE)					\
	execerror("small numeric",NAME)
#define CHECKEMPTYSET(SET,NAME)					\
    if (SET == 0)						\
	execerror("non-empty set",NAME)
#define CHECKEMPTYSTRING(STRING,NAME)				\
    if (*STRING == '\0')					\
	execerror("non-empty string",NAME)
#define CHECKEMPTYLIST(LIST,NAME)				\
    if (LIST == NULL)						\
	execerror("non-empty list",NAME)
#define INDEXTOOLARGE(NAME)					\
    execerror("smaller index",NAME)
#define BADAGGREGATE(NAME)					\
    execerror("aggregate parameter",NAME)
#define BADDATA(NAME)						\
    execerror("different type",NAME)

#define DMP dump->u.lis
#define DMP1 dump1->u.lis
#define DMP2 dump2->u.lis
#define DMP3 dump3->u.lis
#define DMP4 dump4->u.lis
#define DMP5 dump5->u.lis
#define SAVESTACK  dump = LIST_NEWNODE(stk,dump)
#define SAVED1 DMP
#define SAVED2 DMP->next
#define SAVED3 DMP->next->next
#define SAVED4 DMP->next->next->next
#define SAVED5 DMP->next->next->next->next
#define SAVED6 DMP->next->next->next->next->next

#define POP(X) X = X->next

#define NULLARY(CONSTRUCTOR,VALUE)                              \
    stk = CONSTRUCTOR(VALUE, stk)
#define UNARY(CONSTRUCTOR,VALUE)                                \
    stk = CONSTRUCTOR(VALUE, stk->next)
#define BINARY(CONSTRUCTOR,VALUE)                               \
    stk = CONSTRUCTOR(VALUE, stk->next->next)
#define GNULLARY(TYPE,VALUE)                                    \
    stk = newnode(TYPE,(VALUE),stk)
#define GUNARY(TYPE,VALUE)                                      \
    stk = newnode(TYPE,(VALUE),stk->next)
#define GBINARY(TYPE,VALUE)                                     \
    stk = newnode(TYPE,(VALUE),stk->next->next)
#define GTERNARY(TYPE,VALUE)					\
    stk = newnode(TYPE,(VALUE),stk->next->next->next)

#define GETSTRING(NODE)						\
  ( NODE->op == STRING_  ?  NODE->u.str :			\
   (NODE->op == USR_  ?  NODE->u.ent->name :			\
    opername(NODE->op) ) )

/* - - - -  O P E R A N D S   - - - - */

#define PUSH(PROCEDURE,CONSTRUCTOR,VALUE)			\
PRIVATE void PROCEDURE()					\
{   NULLARY(CONSTRUCTOR,VALUE); }
PUSH(true_,BOOLEAN_NEWNODE,1L)				/* constants	*/
PUSH(false_,BOOLEAN_NEWNODE,0L)
PUSH(setsize_,INTEGER_NEWNODE,(long)SETSIZE)
PUSH(maxint_,INTEGER_NEWNODE,(long)MAXINT)
PUSH(symtabmax_,INTEGER_NEWNODE,(long)SYMTABMAX)
PUSH(memorymax_,INTEGER_NEWNODE,(long)MEMORYMAX)
PUSH(stdin_, FILE_NEWNODE, stdin)
PUSH(stdout_, FILE_NEWNODE, stdout)
PUSH(stderr_, FILE_NEWNODE, stderr)
PUSH(dump_,LIST_NEWNODE,dump)				/* variables	*/
PUSH(conts_,LIST_NEWNODE,LIST_NEWNODE(conts->u.lis->next,conts->next))
PUSH(symtabindex_,INTEGER_NEWNODE,(long)LOC2INT(symtabindex))
/* FIXME: Use /dev/random on Unix or CryptGenRandom on Windows */
PUSH(rand_, INTEGER_NEWNODE, (long)rand())
/* this is now in utils.c
PUSH(memoryindex_,INTEGER_NEWNODE,MEM2INT(memoryindex))
*/
PUSH(echo_,INTEGER_NEWNODE,(long)echoflag)
PUSH(autoput_,INTEGER_NEWNODE,(long)autoput)
PUSH(undeferror_,INTEGER_NEWNODE,(long)undeferror)
PUSH(clock_,INTEGER_NEWNODE,(long)(clock() - startclock))
PUSH(time_,INTEGER_NEWNODE,(long)time(NULL))
PUSH(argc_,INTEGER_NEWNODE,(long)g_argc)

PUBLIC void stack_(void)
{ NULLARY(LIST_NEWNODE, stk); }

/* - - - - -   O P E R A T O R S   - - - - - */

PRIVATE void id_()
{
    /* do nothing */
}

PRIVATE void unstack_()
{
    ONEPARAM("unstack");
    LIST("unstack");
    stk = stk->u.lis;
}

/*
PRIVATE void newstack_()
{
    stk = NULL;
}
*/

/* - - -   STACK   - - - */

PRIVATE void name_()
{
    ONEPARAM("name");
    UNARY(STRING_NEWNODE, stk->op == USR_ ?
		   stk->u.ent->name : opername(stk->op));
}

PRIVATE void intern_()
{
    char *p;
    ONEPARAM("intern");
    STRING("intern");
    strncpy(id, stk->u.str, sizeof(id));
    id[sizeof(id) - 1] = 0;
    hashvalue = 0;
    for (p = id; *p; p++) hashvalue += *p;
    hashvalue %= HASHSIZE;
    lookup();
    if (location < firstlibra)
	{ bucket.proc = location->u.proc;
	GUNARY(LOC2INT(location), bucket); }
	else UNARY(USR_NEWNODE, location);
}

PRIVATE void getenv_()
{
    ONEPARAM("getenv");
    STRING("getenv");
    UNARY(STRING_NEWNODE, getenv(stk->u.str)); }

PRIVATE void body_()
{
    ONEPARAM("body");
    USERDEF("body");
    UNARY(LIST_NEWNODE,stk->u.ent->u.body);
}

PRIVATE void pop_()
{
    ONEPARAM("pop");
    POP(stk);
}

PRIVATE void swap_()
{
    TWOPARAMS("swap");
    SAVESTACK;
    GBINARY(SAVED1->op,SAVED1->u);
    GNULLARY(SAVED2->op,SAVED2->u);
    POP(dump);
}

PRIVATE void rollup_()
{
    THREEPARAMS("rollup");
    SAVESTACK;
    GTERNARY(SAVED1->op,SAVED1->u);
    GNULLARY(SAVED3->op,SAVED3->u);
    GNULLARY(SAVED2->op,SAVED2->u);
    POP(dump);
}

PRIVATE void rolldown_()
{
    THREEPARAMS("rolldown");
    SAVESTACK;
    GTERNARY(SAVED2->op,SAVED2->u);
    GNULLARY(SAVED1->op,SAVED1->u);
    GNULLARY(SAVED3->op,SAVED3->u);
    POP(dump);
}

PRIVATE void rotate_()
{
    THREEPARAMS("rotate");
    SAVESTACK;
    GTERNARY(SAVED1->op,SAVED1->u);
    GNULLARY(SAVED2->op,SAVED2->u);
    GNULLARY(SAVED3->op,SAVED3->u);
    POP(dump);
}

PRIVATE void dup_()
{
    ONEPARAM("dup");
    GNULLARY(stk->op,stk->u);
}

#define DIPPED(PROCEDURE,NAME,PARAMCOUNT,ARGUMENT)              \
PRIVATE void PROCEDURE()                                        \
{   PARAMCOUNT(NAME);                                           \
    SAVESTACK;                                                  \
    POP(stk);                                                   \
    ARGUMENT();                                                 \
    GNULLARY(SAVED1->op,SAVED1->u);                             \
    POP(dump);                                                  \
}

DIPPED(popd_,"popd",TWOPARAMS,pop_)
DIPPED(dupd_,"dupd",TWOPARAMS,dup_)
DIPPED(swapd_,"swapd",THREEPARAMS,swap_)
DIPPED(rolldownd_,"rolldownd",FOURPARAMS,rolldown_)
DIPPED(rollupd_,"rollupd",FOURPARAMS,rollup_)
DIPPED(rotated_,"rotated",FOURPARAMS,rotate_)

/* - - -   BOOLEAN   - - - */

#define ANDORXOR(PROCEDURE,NAME,OPER1,OPER2)			\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    SAME2TYPES(NAME);						\
    switch (stk->next->op)					\
      { case SET_:						\
	    BINARY(SET_NEWNODE,(long)(stk->next->u.set OPER1 stk->u.set)); \
	    return;						\
	case BOOLEAN_: case CHAR_: case INTEGER_: case LIST_:	\
	    BINARY(BOOLEAN_NEWNODE,(long)(stk->next->u.num OPER2 stk->u.num)); \
	    return;						\
	default:						\
	    BADDATA(NAME); } }

ANDORXOR(and_,"and",&,&&)
ANDORXOR(or_,"or",|,||)
ANDORXOR(xor_,"xor",^,!=)

/* - - -   INTEGER   - - - */

#define ORDCHR(PROCEDURE,NAME,RESULTTYP)			\
PRIVATE void PROCEDURE()					\
{   ONEPARAM(NAME);						\
    NUMERICTYPE(NAME);						\
    UNARY(RESULTTYP,stk->u.num);				\
}

ORDCHR(ord_,"ord",INTEGER_NEWNODE)
ORDCHR(chr_,"chr",CHAR_NEWNODE)

PRIVATE void abs_()
{
    ONEPARAM("abs");
/* start new */
    FLOAT("abs");
    if (stk->op == INTEGER_)
      { if (stk->u.num >= 0) return;
        else { UNARY(INTEGER_NEWNODE, - stk->u.num); return; } }
/* end new */
    FLOAT_U(fabs);
    INTEGER("abs");
    if (stk->u.num < 0) UNARY(INTEGER_NEWNODE, - stk->u.num);
}

PRIVATE double fsgn(double f)
{   if (f < 0) return -1.0;
    else if (f > 0) return 1.0;
    else return 0.0;
}

PRIVATE void sign_()
{
    ONEPARAM("sign");
/* start new */
    FLOAT("sign");
    if (stk->op == INTEGER_)
      { long i = stk->u.num;
	if (i == 0 || i == 1) return;
	else { UNARY(INTEGER_NEWNODE, i > 0 ? 1 : -1); return; } }
/* end new */
    FLOAT_U(fsgn);
    INTEGER("sign");
    if (stk->u.num < 0) UNARY(INTEGER_NEWNODE,-1L);
    else if (stk->u.num > 0) UNARY(INTEGER_NEWNODE,1L);
}

PRIVATE void neg_()
{
    ONEPARAM("neg");
    FLOAT_U(-);
    INTEGER("neg");
    UNARY(INTEGER_NEWNODE, -stk->u.num);
}

/* probably no longer needed:
#define MULDIV(PROCEDURE,NAME,OPER,CHECK)			\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    FLOAT_I(OPER);						\
    INTEGERS2(NAME);						\
    CHECK;							\
    BINARY(INTEGER_NEWNODE,stk->next->u.num OPER stk->u.num); }

MULDIV(mul_,"*",*,)
MULDIV(divide_,"/",/,CHECKZERO("/"))
*/
PRIVATE void mul_()
{
    TWOPARAMS("*");
    FLOAT_I(*);
    INTEGERS2("*");
    BINARY(INTEGER_NEWNODE,stk->next->u.num * stk->u.num);
}

PRIVATE void divide_()
{
    TWOPARAMS("/");
    if ((stk->op == FLOAT_   && stk->u.dbl == 0.0) ||
	(stk->op == INTEGER_ && stk->u.num == 0))
      execerror("non-zero divisor","/");
    FLOAT_I(/);
    INTEGERS2("/");
    BINARY(INTEGER_NEWNODE,stk->next->u.num / stk->u.num);
}

PRIVATE void rem_()
{
    TWOPARAMS("rem");
    FLOAT_P(fmod);
    INTEGERS2("rem");
    CHECKZERO("rem");
    BINARY(INTEGER_NEWNODE,stk->next->u.num % stk->u.num); }


PRIVATE void div_()
{
    ldiv_t result;
    TWOPARAMS("div");
    INTEGERS2("div");
    CHECKZERO("div");
    result = ldiv(stk->next->u.num, stk->u.num);
    BINARY(INTEGER_NEWNODE, result.quot);
    NULLARY(INTEGER_NEWNODE, result.rem); }

PRIVATE void strtol_()
{   TWOPARAMS("strtol");
    SAVESTACK;
    INTEGER("strtol");
    POP(stk);
    STRING("strtol");
    UNARY(INTEGER_NEWNODE, strtol(SAVED2->u.str, NULL, SAVED1->u.num));
    POP(dump); }

PRIVATE void strtod_()
{
    ONEPARAM("strtod");
    STRING("strtod");
    UNARY(FLOAT_NEWNODE, strtod(stk->u.str, NULL)); }

PRIVATE void format_()
{   int width, prec;
    char spec;
    char format[7];
    char *result;

    FOURPARAMS("format");
    INTEGER("format");
    INTEGER2("format");
    prec = stk->u.num;
    POP(stk);
    width = stk->u.num;
    POP(stk);
    CHARACTER("format");
    spec = stk->u.num;
    POP(stk);
    if (!strchr("dioxX", spec))
	execerror("one of: d i o x X", "format");
    strcpy(format, "%*.*ld");
    format[5] = spec;
    result = malloc(INPLINEMAX);		/* should be sufficient */
    NUMERICTYPE("format");
    sprintf(result, format, width, prec, stk->u.num);
    UNARY(STRING_NEWNODE, result);
    return; }

PRIVATE void formatf_()
{   int width, prec;
    char spec;
    char format[7];
    char *result;

    FOURPARAMS("format");
    INTEGER("format");
    INTEGER2("format");
    prec = stk->u.num;
    POP(stk);
    width = stk->u.num;
    POP(stk);
    CHARACTER("format");
    spec = stk->u.num;
    POP(stk);
    if (!strchr("eEfgG", spec))
	execerror("one of: e E f g G", "format");
    strcpy(format, "%*.*lg");
    format[5] = spec;
    result = malloc(INPLINEMAX);		/* should be sufficient */
    FLOAT("format");
    sprintf(result, format, width, prec, stk->u.dbl);
    UNARY(STRING_NEWNODE, result);
    return; }

/* - - -   TIME   - - - */

#define UNMKTIME(PROCEDURE,NAME,FUNC)				\
PRIVATE void PROCEDURE()					\
{   struct tm *t;						\
    long wday;							\
    time_t timval;						\
    ONEPARAM(NAME);						\
    INTEGER(NAME);						\
    timval = stk->u.num;					\
    t = FUNC(&timval);						\
    wday = t->tm_wday;						\
    if (wday == 0) wday = 7;					\
    dump1 = LIST_NEWNODE(NULL, dump1);				\
    DMP1 = INTEGER_NEWNODE(wday, DMP1);				\
    DMP1 = INTEGER_NEWNODE((long)t->tm_yday, DMP1);		\
    DMP1 = BOOLEAN_NEWNODE((long)t->tm_isdst, DMP1);		\
    DMP1 = INTEGER_NEWNODE((long)t->tm_sec, DMP1);		\
    DMP1 = INTEGER_NEWNODE((long)t->tm_min, DMP1);		\
    DMP1 = INTEGER_NEWNODE((long)t->tm_hour, DMP1);		\
    DMP1 = INTEGER_NEWNODE((long)t->tm_mday, DMP1);		\
    DMP1 = INTEGER_NEWNODE((long)(t->tm_mon + 1), DMP1);	\
    DMP1 = INTEGER_NEWNODE((long)(t->tm_year + 1900), DMP1);	\
    UNARY(LIST_NEWNODE, DMP1);					\
    POP(dump1);							\
    return; }

UNMKTIME(localtime_,"localtime",localtime)
UNMKTIME(gmtime_,"gmtime",gmtime)

PRIVATE void decode_time(struct tm *t)
{   Node *p;
    t->tm_year = t->tm_mon = t->tm_mday =
	t->tm_hour = t->tm_min = t->tm_sec = t->tm_isdst =
	t->tm_yday = t->tm_wday = 0;
    p = stk->u.lis;
    if (p && p->op == INTEGER_)
	{ t->tm_year = p->u.num - 1900; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_mon = p->u.num - 1; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_mday = p->u.num; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_hour = p->u.num; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_min = p->u.num; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_sec = p->u.num; POP(p); }
    if (p && p->op == BOOLEAN_)
	{ t->tm_isdst = p->u.num; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_yday = p->u.num; POP(p); }
    if (p && p->op == INTEGER_)
	{ t->tm_wday = p->u.num; POP(p); }
    return; }

PRIVATE void mktime_()
{   struct tm t;
    ONEPARAM("mktime");
    LIST("mktime");
    decode_time(&t);
    UNARY(INTEGER_NEWNODE, (long)mktime(&t));
    return; }

PRIVATE void strftime_()
{   struct tm t;
    char *fmt;
    char *result;
    size_t length;
    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = stk->u.str;
    POP(stk);
    LIST("strftime");
    decode_time(&t);
    length = strlen(fmt) * 3 + 1;		/* should be sufficient */
    result = malloc(length);
    strftime(result, length, fmt, &t);
    UNARY(STRING_NEWNODE, result);
    return; }

/* - - -   FLOAT   - - - */

#define UFLOAT(PROCEDURE,NAME,FUNC)				\
PRIVATE void PROCEDURE()					\
{   ONEPARAM(NAME);						\
    FLOAT(NAME);						\
    UNARY(FLOAT_NEWNODE, FUNC(FLOATVAL));			\
    return; }
UFLOAT(acos_,"acos",acos)
UFLOAT(asin_,"asin",asin)
UFLOAT(atan_,"atan",atan)
UFLOAT(ceil_,"ceil",ceil)
UFLOAT(cos_,"cos",cos)
UFLOAT(cosh_,"cosh",cosh)
UFLOAT(exp_,"exp",exp)
UFLOAT(floor_,"floor",floor)
UFLOAT(log_,"log",log)
UFLOAT(log10_,"log10",log10)
UFLOAT(sin_,"sin",sin)
UFLOAT(sinh_,"sinh",sinh)
UFLOAT(sqrt_,"sqrt",sqrt)
UFLOAT(tan_,"tan",tan)
UFLOAT(tanh_,"tanh",tanh)

#define BFLOAT(PROCEDURE,NAME,FUNC)				\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    FLOAT2(NAME);						\
    BINARY(FLOAT_NEWNODE, FUNC(FLOATVAL2, FLOATVAL));		\
    return; }
BFLOAT(atan2_,"atan2",atan2)
BFLOAT(pow_,"pow",pow)

PRIVATE void frexp_()
{   int exp;
    ONEPARAM("frexp");
    FLOAT("frexp");
    UNARY(FLOAT_NEWNODE, frexp(FLOATVAL, &exp));
    NULLARY(INTEGER_NEWNODE, (long)exp);
    return; }

PRIVATE void modf_()
{   double exp;
    ONEPARAM("frexp");
    FLOAT("frexp");
    UNARY(FLOAT_NEWNODE, modf(FLOATVAL, &exp));
    NULLARY(FLOAT_NEWNODE, exp);
    return; }

PRIVATE void ldexp_()
{   long exp;
    TWOPARAMS("ldexp");
    INTEGER("ldexp");
    exp = stk->u.num;
    POP(stk);
    FLOAT("ldexp");
    UNARY(FLOAT_NEWNODE, ldexp(FLOATVAL, (int)exp));
    return; }

PRIVATE void trunc_()
{
    ONEPARAM("trunc");
    FLOAT("trunc");
    UNARY(INTEGER_NEWNODE, (long)FLOATVAL);
}

/* - - -   NUMERIC   - - - */

#define PREDSUCC(PROCEDURE,NAME,OPER)				\
PRIVATE void PROCEDURE()					\
{   ONEPARAM(NAME);						\
    NUMERICTYPE(NAME);						\
    if (stk->op == CHAR_)					\
	UNARY(CHAR_NEWNODE, stk->u.num OPER 1);			\
    else UNARY(INTEGER_NEWNODE, stk->u.num OPER 1); }

PREDSUCC(pred_,"pred",-)
PREDSUCC(succ_,"succ",+)

#define PLUSMINUS(PROCEDURE,NAME,OPER)				\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    FLOAT_I(OPER);						\
    INTEGER(NAME);						\
    NUMERIC2(NAME);						\
    if (stk->next->op == CHAR_)					\
	BINARY(CHAR_NEWNODE, stk->next->u.num OPER stk->u.num);	\
    else BINARY(INTEGER_NEWNODE, stk->next->u.num OPER stk->u.num); }

PLUSMINUS(plus_,"+",+)
PLUSMINUS(minus_,"-",-)

#define MAXMIN(PROCEDURE,NAME,OPER)				\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    if (FLOATABLE2)						\
      { BINARY(FLOAT_NEWNODE,					\
	    FLOATVAL OPER FLOATVAL2 ?				\
	    FLOATVAL2 : FLOATVAL);				\
	return; }						\
    SAME2TYPES(NAME);						\
    NUMERICTYPE(NAME);						\
    if (stk->op == CHAR_)					\
	BINARY(CHAR_NEWNODE,					\
	    stk->u.num OPER stk->next->u.num ?			\
	    stk->next->u.num : stk->u.num);			\
    else BINARY(INTEGER_NEWNODE,				\
	    stk->u.num OPER stk->next->u.num ?			\
	    stk->next->u.num : stk->u.num); }

MAXMIN(max_,"max",<)
MAXMIN(min_,"min",>)

#define COMPREL(PROCEDURE,NAME,CONSTRUCTOR,OPR)			\
PRIVATE void PROCEDURE()					\
  { long comp = 0;						\
    TWOPARAMS(NAME);						\
    switch (stk->op)						\
      { case BOOLEAN_: case CHAR_: case INTEGER_:		\
	    if (FLOATABLE2)					\
		comp = FLOATVAL2 - FLOATVAL OPR 0;		\
	    else						\
		comp = stk->next->u.num - stk->u.num OPR 0;	\
	    break;						\
	case FLOAT_:						\
	    if (FLOATABLE2)					\
		comp = FLOATVAL2 - FLOATVAL OPR 0;		\
	    else						\
		comp = 0;					\
	    break;						\
	case SET_:						\
	  { int i = 0;						\
	    while ( i < SETSIZE &&				\
		    ( (stk->next->u.set & 1 << i) ==		\
		      (stk->u.set & 1 << i) )  )		\
		++i;						\
	    if (i == SETSIZE) i = 0; else ++i;			\
	    if (!(stk->u.set & 1 << i)) i = -i;			\
	    comp = i OPR 0;					\
	    break; }						\
	case LIST_:						\
	    BADDATA(NAME);					\
	default:						\
	    if (stk->next->op == LIST_)				\
	      BADDATA(NAME);					\
	    comp = strcmp(GETSTRING(stk->next), GETSTRING(stk))	\
		   OPR 0;					\
	    break; }						\
    stk = CONSTRUCTOR(comp, stk->next->next); }

COMPREL(eql_,"=",BOOLEAN_NEWNODE,==)
COMPREL(neql_,"!=",BOOLEAN_NEWNODE,!=)
COMPREL(less_,"<",BOOLEAN_NEWNODE,<)
COMPREL(leql_,"<=",BOOLEAN_NEWNODE,<=)
COMPREL(greater_,">",BOOLEAN_NEWNODE,>)
COMPREL(geql_,">=",BOOLEAN_NEWNODE,>=)
COMPREL(compare_,"compare",INTEGER_NEWNODE,+)

/* - - -   FILES AND STREAMS   - - - */

PRIVATE void fopen_()
{
    TWOPARAMS("fopen");
    STRING("fopen");
    STRING2("fopen");
    BINARY(FILE_NEWNODE, fopen(stk->next->u.str, stk->u.str));
    return; }

PRIVATE void fclose_()
{
    ONEPARAM("fclose");
    if (stk->op == FILE_ && stk->u.fil == NULL)
	{ POP(stk);  return; }
    FILE("fclose");
    fclose(stk->u.fil);
    POP(stk);
    return; }

PRIVATE void fflush_()
{
    ONEPARAM("fflush");
    FILE("fflush");
    fflush(stk->u.fil);
    return; }

PRIVATE void fremove_()
{
    ONEPARAM("fremove");
    STRING("fremove");
    UNARY(BOOLEAN_NEWNODE, (long)!remove(stk->u.str));
    return; }

PRIVATE void frename_()
{
    TWOPARAMS("frename");
    STRING("frename");
    STRING2("frename");
    BINARY(BOOLEAN_NEWNODE, (long)!rename(stk->next->u.str, stk->u.str));
    return; }

#define FILEGET(PROCEDURE,NAME,CONSTRUCTOR,EXPR)		\
PRIVATE void PROCEDURE()					\
{   ONEPARAM(NAME);						\
    FILE(NAME);							\
    NULLARY(CONSTRUCTOR,EXPR);					\
    return; }

FILEGET(feof_,"feof",BOOLEAN_NEWNODE,(long)feof(stk->u.fil))
FILEGET(ferror_,"ferror",BOOLEAN_NEWNODE,(long)ferror(stk->u.fil))
FILEGET(fgetch_,"fgetch",CHAR_NEWNODE,(long)getc(stk->u.fil))
FILEGET(ftell_,"ftell",INTEGER_NEWNODE,ftell(stk->u.fil))

PRIVATE void fgets_()
{   int length = 0;
    int size = INPLINEMAX;
    char *buff = NULL;
    ONEPARAM("fgets");
    FILE("fgets");
    for (;;)
      { buff = realloc(buff, size);
	if (fgets(buff + length, size - length, stk->u.fil) == NULL)
	    { buff[length] = 0; break; }
	if (strchr(buff, '\n')) break;
	length += strlen(buff);
	size = size * 2; }
    NULLARY(STRING_NEWNODE, buff);
    return; }

PRIVATE void fput_()
{   FILE *stm=0;
    TWOPARAMS("fput");
    if (stk->next->op != FILE_ || (stm = stk->next->u.fil) == NULL)
	execerror("file", "fput");
    writefactor(stk, stm);
    fprintf(stm, " ");
    POP(stk);
    return; }

PRIVATE void fputch_()
{   int ch;
    TWOPARAMS("fputch");
    INTEGER("fputch");
    ch = stk->u.num;
    POP(stk);
    FILE("fputch");
    putc(ch, stk->u.fil);
    return; }

PRIVATE void fputchars_() /* suggested by Heiko Kuhrt, as "fputstring_" */
{   FILE *stm=0;
    TWOPARAMS("fputchars");
    if (stk->next->op != FILE_ || (stm = stk->next->u.fil) == NULL)
        execerror("file", "fputchars");
    fprintf(stm,"%s",stk->u.str);
    POP(stk);
    return; }

PRIVATE void fread_()
{   unsigned char *buf;
    long count;

    TWOPARAMS("fread");
    INTEGER("fread");
    count = stk->u.num;
    POP(stk);
    FILE("fread");
    buf = malloc(count);
    dump1 = LIST_NEWNODE(NULL, dump1);
    for (count = fread(buf, (size_t)1, (size_t)count, stk->u.fil) - 1;
	     count >= 0; count--)
	DMP1 = INTEGER_NEWNODE((long)buf[count], DMP1);
    free(buf);
    UNARY(LIST_NEWNODE, DMP1);
    POP(dump1);
    return; }

PRIVATE void fwrite_()
{   int length;
    int i;
    unsigned char *buff;
    Node *n;
    TWOPARAMS("fwrite");
    LIST("fwrite");
    for (n = stk->u.lis, length = 0; n; n = n->next, length++)
	if (n->op != INTEGER_) execerror("numeric list", "fwrite");
    buff = malloc(length);
    for (n = stk->u.lis, i = 0; n; n = n->next, i++)
	buff[i] = n->u.num;
    POP(stk);
    FILE("fwrite");
    fwrite(buff, (size_t)length, (size_t)1, stk->u.fil);
    free(buff);
    return; }

PRIVATE void fseek_()
{   long pos;
    int whence;
    THREEPARAMS("fseek");
    INTEGER("fseek");
    INTEGER2("fseek");
    whence = stk->u.num;
    POP(stk);
    pos = stk->u.num;
    POP(stk);
    FILE("fseek");
    NULLARY(BOOLEAN_NEWNODE, (long)!!fseek(stk->u.fil, pos, whence));
    return; }

/* - - -   AGGREGATES   - - - */

PRIVATE void first_()
{
    ONEPARAM("first");
    switch (stk->op)
      { case LIST_:
	    CHECKEMPTYLIST(stk->u.lis,"first");
	    GUNARY(stk->u.lis->op,stk->u.lis->u);
	    return;
	case STRING_:
	    CHECKEMPTYSTRING(stk->u.str,"first");
	    UNARY(CHAR_NEWNODE,(long)*(stk->u.str));
	    return;
	case SET_:
	  { long i = 0;
	    CHECKEMPTYSET(stk->u.set,"first");
	    while (!(stk->u.set & (1 << i))) i++;
	    UNARY(INTEGER_NEWNODE,i);
	    return; }
	default:
	    BADAGGREGATE("first"); }
}

PRIVATE void rest_()
{
    ONEPARAM("rest");
    switch (stk->op)
      { case SET_:
	  { int i = 0;
	    CHECKEMPTYSET(stk->u.set,"rest");
	    while (!(stk->u.set & (1 << i))) i++;
	    UNARY(SET_NEWNODE,stk->u.set & ~(1 << i));
	    break; }
	case STRING_:
	  { char *s = stk->u.str;
	    CHECKEMPTYSTRING(s,"rest");
	    UNARY(STRING_NEWNODE, ++s);
	    break; }
	case LIST_:
	    CHECKEMPTYLIST(stk->u.lis,"rest");
	    UNARY(LIST_NEWNODE,stk->u.lis->next);
	    return;
	default:
	    BADAGGREGATE("rest"); }
}

PRIVATE void uncons_()
{
    ONEPARAM("uncons");
    switch (stk->op)
      { case SET_:
	  { long i = 0; long set = stk->u.set;
	    CHECKEMPTYSET(set,"uncons");
	    while (!(set & (1 << i))) i++;
	    UNARY(INTEGER_NEWNODE,i);
	    NULLARY(SET_NEWNODE,set & ~(1 << i));
	    break; }
	case STRING_:
	  { char *s = stk->u.str;
	    CHECKEMPTYSTRING(s,"uncons");
	    UNARY(CHAR_NEWNODE,(long)*s);
	    NULLARY(STRING_NEWNODE,++s);
	    break; }
	case LIST_:
	    SAVESTACK;
	    CHECKEMPTYLIST(SAVED1->u.lis,"uncons");
	    GUNARY(SAVED1->u.lis->op,SAVED1->u.lis->u);
	    NULLARY(LIST_NEWNODE,SAVED1->u.lis->next);
	    POP(dump);
	    return;
	default:
	    BADAGGREGATE("uncons"); }
}

PRIVATE void unswons_()
{
    ONEPARAM("unswons");
    switch (stk->op)
      { case SET_:
	  { long i = 0; long set = stk->u.set;
	    CHECKEMPTYSET(set,"unswons");
	    while (!(set & (1 << i))) i++;
	    UNARY(SET_NEWNODE,set & ~(1 << i));
	    NULLARY(INTEGER_NEWNODE,i);
	    break; }
	case STRING_:
	  { char *s = stk->u.str;
	    CHECKEMPTYSTRING(s,"unswons");
	    UNARY(STRING_NEWNODE,++s);
	    NULLARY(CHAR_NEWNODE,(long)*(--s));
	    break; }
	case LIST_:
	    SAVESTACK;
	    CHECKEMPTYLIST(SAVED1->u.lis,"unswons");
	    UNARY(LIST_NEWNODE,SAVED1->u.lis->next);
	    GNULLARY(SAVED1->u.lis->op,SAVED1->u.lis->u);
	    POP(dump);
	    return;
	default:
	    BADAGGREGATE("unswons"); }
}

PRIVATE long equal_aux(); /* forward */

PRIVATE int equal_list_aux(n1,n2)
Node *n1, *n2;
{
    if (n1 == NULL && n2 == NULL) return 1;
    if (n1 == NULL || n2 == NULL) return 0;
    if (equal_aux(n1,n2))
	return equal_list_aux(n1->next,n2->next);
    else return 0;
}

PRIVATE long equal_aux(n1,n2)
Node *n1, *n2;
{
    if (n1 == NULL && n2 == NULL) return 1;
    if (n1 == NULL || n2 == NULL) return 0;
    switch (n1->op)
      { case BOOLEAN_: case CHAR_: case INTEGER_:
	    if (n2->op != BOOLEAN_ && n2->op != CHAR_ &&
		n2->op != INTEGER_)
		return 0;
	    return n1->u.num == n2->u.num;
	case SET_ :
	    if (n2->op != SET_) return 0;
	    return n1->u.num == n2->u.num;
	case LIST_ :
	    if (n2->op != LIST_) return 0;
	    return equal_list_aux(n1->u.lis,n2->u.lis);
	default:
	    return strcmp(GETSTRING(n1),GETSTRING(n2)) == 0; }
}

PRIVATE void equal_()
{
    TWOPARAMS("equal");
    BINARY(BOOLEAN_NEWNODE,equal_aux(stk,stk->next));
}

#define INHAS(PROCEDURE,NAME,AGGR,ELEM)				\
PRIVATE void PROCEDURE()					\
{   int found = 0;						\
    TWOPARAMS(NAME);						\
    switch (AGGR->op)						\
      { case SET_:						\
	    found = ((AGGR->u.set) & (1 << ELEM->u.num)) > 0;	\
	    break;						\
	case STRING_:						\
	  { char *s;						\
	    for (s = AGGR->u.str;				\
		 *s != '\0' && *s != ELEM->u.num;		\
		 s++);						\
	    found = *s != '\0';					\
	    break; }						\
	case LIST_:						\
	  { Node *n = AGGR->u.lis;				\
	    while (n != NULL && n->u.num != ELEM->u.num)	\
		n = n->next;					\
	    found = n != NULL;					\
	    break; }						\
	default:						\
	    BADAGGREGATE(NAME); }				\
    BINARY(BOOLEAN_NEWNODE,(long)found);			\
}

INHAS(in_,"in",stk,stk->next)
INHAS(has_,"has",stk->next,stk)

#define OF_AT(PROCEDURE,NAME,AGGR,INDEX)			\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    if (INDEX->op != INTEGER_ || INDEX->u.num < 0)		\
	execerror("non-negative integer", NAME);		\
    switch (AGGR->op)						\
      { case SET_:						\
	  { long i; int indx = INDEX->u.num;			\
	    CHECKEMPTYSET(AGGR->u.set,NAME);			\
	    for (i = 0; i < SETSIZE; i++)			\
	      { if (AGGR->u.set & (1 << i))			\
		  { if (indx == 0)				\
			{BINARY(INTEGER_NEWNODE,i); return;}	\
		    indx--; } }					\
	    INDEXTOOLARGE(NAME);				\
	    return; }						\
	case STRING_:						\
	    if ((int)strlen(AGGR->u.str) < INDEX->u.num)	\
		INDEXTOOLARGE(NAME);				\
	    BINARY(CHAR_NEWNODE,(long)AGGR->u.str[INDEX->u.num]);	\
	    return;						\
	case LIST_:						\
	  { Node *n = AGGR->u.lis;  int i  = INDEX->u.num;	\
	    CHECKEMPTYLIST(n,NAME);				\
	    while (i > 0)					\
	      { if (n->next == NULL)				\
		    INDEXTOOLARGE(NAME);			\
		n = n->next; i--; }				\
	    GBINARY(n->op,n->u);				\
	    return; }						\
	default:						\
	    BADAGGREGATE(NAME); }				\
}

OF_AT(of_,"of",stk,stk->next)
OF_AT(at_,"at",stk->next,stk)

PRIVATE void choice_()
{
    THREEPARAMS("choice");
    if (stk->next->next->u.num)
	 stk = newnode(stk->next->op,stk->next->u,
		       stk->next->next->next);
    else stk = newnode(stk->op,stk->u,
		       stk->next->next->next);
}

PRIVATE void case_()
{
    Node *n;

    TWOPARAMS("case");
    LIST("case");
    n = stk->u.lis;
    CHECKEMPTYLIST(n,"case");
    while ( n->next != NULL &&
	    n->u.lis->u.num != stk->next->u.num )
	n = n->next;
/*
    printf("case : now execute : ");
    writefactor(n->u.lis, stdout); printf("\n");
    stk = stk->next->next;
    exeterm(n->next != NULL ? n->u.lis->next : n->u.lis);
*/
    if (n->next != NULL)
	{stk = stk->next->next; exeterm(n->u.lis->next);}
    else
	{stk = stk->next;       exeterm(n->u.lis);}
}

PRIVATE void opcase_()
{
    Node *n;
    ONEPARAM("opcase");
    LIST("opcase");
    n = stk->u.lis;
    CHECKEMPTYLIST(n,"opcase");
    while ( n->next != NULL &&
	    n->op == LIST_ &&
	    n->u.lis->op != stk->next->op )
	n = n->next;
    CHECKLIST(n->op,"opcase");
    UNARY(LIST_NEWNODE,
	n->next != NULL ? n->u.lis->next : n->u.lis);
}

#define CONS_SWONS(PROCEDURE,NAME,AGGR,ELEM)			\
PRIVATE void PROCEDURE()					\
{   TWOPARAMS(NAME);						\
    switch (AGGR->op)						\
      { case LIST_:						\
	    BINARY(LIST_NEWNODE,newnode(ELEM->op,		\
				 ELEM->u,AGGR->u.lis));		\
	    break;						\
	case SET_:						\
	    CHECKSETMEMBER(ELEM,NAME);				\
	    BINARY(SET_NEWNODE,AGGR->u.set | (1 << ELEM->u.num));	\
	    break;						\
	case STRING_:						\
	  { char *s;						\
	    if (ELEM->op != CHAR_)				\
		execerror("character", NAME);			\
	    s = (char *) malloc(strlen(AGGR->u.str) + 2);	\
	    s[0] = ELEM->u.num;					\
	    strcpy(s + 1,AGGR->u.str);				\
	    BINARY(STRING_NEWNODE,s);				\
	    break; }						\
	default:						\
	    BADAGGREGATE(NAME); }				\
}

CONS_SWONS(cons_,"cons",stk,stk->next)
CONS_SWONS(swons_,"swons",stk->next,stk)

PRIVATE void drop_()
{   int n = stk->u.num;
    TWOPARAMS("drop");
    switch (stk->next->op)
      { case SET_:
	  { int i; long result = 0;
	    for (i = 0; i < SETSIZE; i++)
	        if (stk->next->u.set & (1 << i))
		  { if (n < 1) result = result | (1 << i);
		    else n--; }
	    BINARY(SET_NEWNODE,result);
	    return; }
	case STRING_:
	  { char *result = stk->next->u.str;
	    while (n-- > 0  &&  *result != '\0') ++result;
	    BINARY(STRING_NEWNODE,result);
	    return; }
	case LIST_:
	  { Node *result = stk->next->u.lis;
	    while (n-- > 0 && result != NULL) result = result->next;
	    BINARY(LIST_NEWNODE,result);
	    return; }
	default:
	    BADAGGREGATE("drop"); }
}

PRIVATE void take_()
{   int n = stk->u.num;
    TWOPARAMS("take");
    switch (stk->next->op)
      { case SET_:
	  { int i; long result = 0;
	    for (i = 0; i < SETSIZE; i++)
	        if (stk->next->u.set & (1 << i))
		  { if (n > 0)
		      { --n;  result = result | (1 << i); }
		    else break; }
	    BINARY(SET_NEWNODE,result);
	    return; }
	case STRING_:
	  { int i; char *old, *p, *result;
	    i = stk->u.num;
	    old = stk->next->u.str;
	    POP(stk);
	    /* do not swap the order of the next two statements ! ! ! */
	    if (i < 0) i = 0;
	    if (i > (int)strlen(old)) return; /* the old string unchanged */
	    p = result = (char *) malloc(strlen(old) - i + 1);
	    while (i-- > 0)  *p++ = *old++;
	    UNARY(STRING_NEWNODE,result);
	    return; }
	case LIST_:
	  { int i = stk->u.num;
	    if (i < 1) { BINARY(LIST_NEWNODE,NULL); return; } /* null string */
	    dump1 = newnode(LIST_,stk->next->u,dump1);	/* old  */
	    dump2 = LIST_NEWNODE(0L, dump2);		/* head */
	    dump3 = LIST_NEWNODE(0L, dump3);		/* last */
	    while (DMP1 != NULL && i-- > 0)
	      { if (DMP2 == NULL)			/* first */
		  { DMP2 = newnode(DMP1->op,DMP1->u,NULL);
		    DMP3 = DMP2; }
		else					/* further */
		  { DMP3->next = newnode(DMP1->op,DMP1->u,NULL);
		    DMP3 = DMP3->next; }
		DMP1 = DMP1->next; }
	    DMP3->next = NULL;
	    BINARY(LIST_NEWNODE,DMP2);
	    POP(dump1); POP(dump2); POP(dump3);
	    return; }
	default:
	    BADAGGREGATE("take"); }
}

PRIVATE void concat_()
{
    TWOPARAMS("concat");
    SAME2TYPES("concat");
    switch (stk->op)
      { case SET_:
	    BINARY(SET_NEWNODE,stk->next->u.set | stk->u.set);
	    return;
	case STRING_:
	  { char *s, *p;
	    s = p = (char *)malloc(strlen(stk->next->u.str) +
				   strlen(stk->u.str) + 1);
	    while ((*p++ = *(stk->next->u.str)++) != '\0');
	    --p; /* don't want terminating null */
	    while ((*p++ = *(stk->u.str)++) != '\0');
	    BINARY(STRING_NEWNODE,s);
	    return; }
	case LIST_:
	    if (stk->next->u.lis == NULL)
	      { BINARY(LIST_NEWNODE,stk->u.lis); return; }
	    dump1 = LIST_NEWNODE(stk->next->u.lis,dump1);/* old  */
	    dump2 = LIST_NEWNODE(0L,dump2);		 /* head */
	    dump3 = LIST_NEWNODE(0L,dump3);		 /* last */
	    while (DMP1 != NULL)
	      { if (DMP2 == NULL)			/* first */
		  { DMP2 =
			newnode(DMP1->op,
			    DMP1->u,NULL);
		    DMP3 = DMP2; }
		else					/* further */
		  { DMP3->next =
			newnode(DMP1->op,
			    DMP1->u,NULL);
		    DMP3 = DMP3->next; };
		DMP1 = DMP1->next; }
	    DMP3->next = stk->u.lis;
	    BINARY(LIST_NEWNODE,DMP2);
	    POP(dump1);
	    POP(dump2);
	    POP(dump3);
	    return;
	default:
	    BADAGGREGATE("concat"); };
}

PRIVATE void enconcat_()
{
    THREEPARAMS("enconcat");
    SAME2TYPES("enconcat");
    swapd_(); cons_(); concat_();
}

PRIVATE void null_()
{
    ONEPARAM("null");
    switch (stk->op)
      { case STRING_:
	    UNARY(BOOLEAN_NEWNODE, (long)(*(stk->u.str) == '\0'));
	    break;
	case FLOAT_:
	    UNARY(BOOLEAN_NEWNODE, (long)(stk->u.dbl == 0.0));
	    break;
	case FILE_:
	    UNARY(BOOLEAN_NEWNODE, (long)(stk->u.fil == NULL));
	    break;
	default:
	    UNARY(BOOLEAN_NEWNODE, (long)(! stk->u.num)); }
}

PRIVATE void not_()
{
    ONEPARAM("not");
    switch (stk->op)
      { case SET_:
	    UNARY(SET_NEWNODE,~ stk->u.set);
	    break;
	case STRING_:
	    UNARY(BOOLEAN_NEWNODE,(long)(*(stk->u.str) != '\0'));
	    break;
	case BOOLEAN_: case CHAR_: case INTEGER_: case LIST_:
	    UNARY(BOOLEAN_NEWNODE, (long)(! stk->u.num));
	    break;
	default:
	    BADDATA("not"); }
}

PRIVATE void size_()
{
    long siz = 0;
    ONEPARAM("size");
    switch (stk->op)
      { case SET_:
	  { int i;
	    for (i = 0; i < SETSIZE; i++)
		if (stk->u.set & (1 << i)) siz++;
	    break; }
	case STRING_:
	    siz = strlen(stk->u.str);
	    break;
	case LIST_:
	  { Node *e = stk->u.lis;
	    while (e != NULL) {e = e->next; siz++;};
	    break; }
	default :
	    BADDATA("size"); }
    UNARY(INTEGER_NEWNODE,siz);
}

PRIVATE void small_()
{
    long sml = 0;
    ONEPARAM("small");
    switch (stk->op)
      { case BOOLEAN_: case INTEGER_:
	    sml = stk->u.num < 2;
	    break;
	case SET_:
	    if (stk->u.set == 0) sml = 1; else
	      { int i = 0;
		while  (!(stk->u.set & (1 << i))) i++;
D(		printf("small: first member found is %d\n",i); )
		sml = (stk->u.set & ~(1 << i)) == 0; }
	    break;
	case STRING_:
	    sml = stk->u.str[0] == '\0' || stk->u.str[1] == '\0';
	    break;
	case LIST_:
	    sml = stk->u.lis == NULL || stk->u.lis->next == NULL;
	    break;
	default:
	    BADDATA("small"); }
    UNARY(BOOLEAN_NEWNODE,sml);
}

#define TYPE(PROCEDURE,NAME,REL,TYP)				\
    PRIVATE void PROCEDURE()					\
    {   ONEPARAM(NAME);						\
	UNARY(BOOLEAN_NEWNODE,(long)(stk->op REL TYP)); }

TYPE(integer_,"integer",==,INTEGER_)
TYPE(char_,"char",==,CHAR_)
TYPE(logical_,"logical",==,BOOLEAN_)
TYPE(string_,"string",==,STRING_)
TYPE(set_,"set",==,SET_)
TYPE(list_,"list",==,LIST_)
TYPE(leaf_,"leaf",!=,LIST_)
TYPE(float_,"float",==,FLOAT_)
TYPE(file_,"file",==,FILE_)
TYPE(user_,"user",==,USR_)

#define USETOP(PROCEDURE,NAME,TYPE,BODY)			\
    PRIVATE void PROCEDURE()					\
    { ONEPARAM(NAME); TYPE(NAME); BODY; POP(stk); }

USETOP( put_,"put",ONEPARAM, writefactor(stk, stdout);printf(" "))
USETOP( putch_,"putch",NUMERICTYPE, printf("%c", (char) stk->u.num) )
USETOP( putchars_,"putchars",STRING, printf("%s",stk->u.str) )
USETOP( setecho_,"setecho",NUMERICTYPE, echoflag = stk->u.num )
USETOP( setautoput_,"setautoput",NUMERICTYPE, autoput = stk->u.num )
USETOP( setundeferror_, "setundeferror", NUMERICTYPE, undeferror = stk->u.num )
USETOP( settracegc_,"settracegc",NUMERICTYPE, tracegc = stk->u.num )
USETOP( srand_,"srand",INTEGER, srand((unsigned int) stk->u.num) )
USETOP( include_,"include",STRING, doinclude(stk->u.str) )
USETOP( system_,"system",STRING, system(stk->u.str) )

PRIVATE void undefs_(void)
{
    Entry *i = symtabindex;
    Node *n = 0;
    while (i != symtab)
      { --i;
	if ( (i->name[0] != 0) && (i->name[0] != '_') &&  (i->u.body == NULL) )
	    n = STRING_NEWNODE(i->name, n); }
    stk = LIST_NEWNODE(n,stk);
}

PRIVATE void argv_()
{
    int i;
    dump1 = LIST_NEWNODE(NULL, dump1);
    for(i = g_argc - 1; i >= 0; i--) {
	DMP1 = STRING_NEWNODE(g_argv[i], DMP1);
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(dump1);
    return;
}

PRIVATE void get_()
{
    getsym();
    readfactor();
}

PUBLIC void dummy_(void)
{
    /* never called */
}

#define HELP(PROCEDURE,REL)					\
PRIVATE void PROCEDURE()					\
{   Entry *i = symtabindex;					\
    int column = 0;						\
    int name_length;						\
    while (i != symtab)						\
	if ((--i)->name[0] REL '_')				\
	  { name_length = strlen(i->name) + 1;			\
	    if (column + name_length > 72)			\
	      { printf("\n"); column = 0; }			\
	    printf("%s ", i->name);				\
	    column += name_length; }				\
    printf("\n"); }
HELP(help1_,!=)
HELP(h_help1_,==)

/* - - - - -   C O M B I N A T O R S   - - - - - */

PUBLIC void exeterm(Node *n)
{
    Node *stepper;
start:
    if (n == NULL) return;
    conts = LIST_NEWNODE(n,conts);
    while (conts->u.lis != NULL)
      {
	if (tracegc > 5)
	  { printf("exeterm1: %ld ",(long)conts->u.lis);
	    printnode(conts->u.lis); }
	stepper = conts->u.lis;
	conts->u.lis = conts->u.lis->next;
	switch (stepper->op)
	  { case BOOLEAN_: case CHAR_: case INTEGER_: case FLOAT_:
	    case SET_: case STRING_: case LIST_:
		stk = newnode(stepper->op, stepper->u, stk); break;
	    case USR_:
              if (stepper->u.ent->u.body == NULL && undeferror)
                  execerror("definition", stepper->u.ent->name);
		if (stepper->next == NULL)
		  { POP(conts);
		    n = stepper->u.ent->u.body;
		    goto start; }
		  else exeterm(stepper->u.ent->u.body );
		break;
	    case COPIED_: case ILLEGAL_:
		printf("exeterm: attempting to execute bad node\n");
		printnode(stepper);
		break;
	    default:
D(		printf("trying to do "); )
D(		writefactor(dump1, stdout); )
		(*(stepper->u.proc))(); break; }
	if (tracegc > 5)
	  { printf("exeterm2: %ld ",(long)stepper);
            printnode(stepper); }
/*
	stepper = stepper->next; }
*/
		}
    POP(conts);
D(  printf("after execution, stk is:\n"); )
D(  writeterm(stk, stdout); )
D(  printf("\n"); )
}

PRIVATE void x_()
{
    ONEPARAM("x");
    ONEQUOTE("x");
    exeterm(stk->u.lis);
}

PRIVATE void i_()
{
    ONEPARAM("i");
    ONEQUOTE("i");
    SAVESTACK;
    POP(stk);
    exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void dip_()
{
    TWOPARAMS("dip");
    ONEQUOTE("dip");
    SAVESTACK;
    stk = stk->next->next;
    exeterm(SAVED1->u.lis);
    GNULLARY(SAVED2->op,SAVED2->u);
    POP(dump);
}

#define N_ARY(PROCEDURE,NAME,PARAMCOUNT,TOP)			\
PRIVATE void PROCEDURE()					\
{   PARAMCOUNT(NAME);						\
    ONEQUOTE(NAME);						\
    SAVESTACK;							\
    POP(stk);							\
    exeterm(SAVED1->u.lis);					\
    if (stk == NULL) execerror("value to push",NAME);		\
    stk = newnode(stk->op, stk->u,TOP);				\
    POP(dump);							\
}

N_ARY(nullary_,"nullary",ONEPARAM,SAVED2)
N_ARY(unary_,"unary",TWOPARAMS,SAVED3)
N_ARY(binary_,"binary",THREEPARAMS,SAVED4)
N_ARY(ternary_,"ternary",FOURPARAMS,SAVED5)

/*
PRIVATE void nullary_()
{
    ONEPARAM("nullary");
    SAVESTACK;
    POP(stk);
    exeterm(SAVED1->u.lis);
    stk->next = SAVED2;
    POP(dump);
}
*/

PRIVATE void times_()
{
    int i,n;
    TWOPARAMS("times");
    ONEQUOTE("times");
    INTEGER2("times");
    SAVESTACK;
    stk = stk->next->next;
    n = SAVED2->u.num;
    for (i = 1; i <= n; i++)
	exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void infra_()
{
    TWOPARAMS("infra");
    ONEQUOTE("infra");
    LIST2("infra");
    SAVESTACK;
    stk = SAVED2->u.lis;
    exeterm(SAVED1->u.lis);
    stk = LIST_NEWNODE(stk,SAVED3);
    POP(dump);
}

PRIVATE void app1_()
{
    TWOPARAMS("app1");
    ONEQUOTE("app1");
    SAVESTACK;
    POP(stk);
    exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void cleave_()
{			/*  X [P1] [P2] cleave ==>  X1 X2	*/
    THREEPARAMS("cleave");
    TWOQUOTES("cleave");
    SAVESTACK;
    stk = SAVED3;
    exeterm(SAVED2->u.lis);			/* [P1]		*/
    dump1 = newnode(stk->op,stk->u,dump1);	/*  X1		*/
    stk = SAVED3;
    exeterm(SAVED1->u.lis);			/* [P2]		*/
    dump1 = newnode(stk->op,stk->u,dump1);	/*  X2		*/
    stk = dump1; dump1 = dump1->next->next; stk->next->next = SAVED4;
    POP(dump);
}

PRIVATE void app11_()
{
    THREEPARAMS("app11");
    ONEQUOTE("app11");
    app1_();
    stk->next = stk->next->next;
}

PRIVATE void unary2_()
{			/*   Y  Z  [P]  unary2     ==>  Y'  Z'  */
    THREEPARAMS("unary2");
    ONEQUOTE("unary2");
    SAVESTACK;
    stk = SAVED2->next;				/* just Y on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(Y) */
    stk = newnode(SAVED2->op,SAVED2->u,
	  SAVED3->next);			/* just Z on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(Z) */
    stk = dump1; dump1 = dump1->next->next; stk->next->next = SAVED4;
    POP(dump);
}

PRIVATE void unary3_()
{			/*  X Y Z [P]  unary3    ==>  X' Y' Z'	*/
    FOURPARAMS("unary3");
    ONEQUOTE("unary3");
    SAVESTACK;
    stk = SAVED3->next;				/* just X on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save p(X) */
    stk = newnode(SAVED3->op,SAVED3->u,
	  SAVED4->next);			/* just Y on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(Y) */
    stk = newnode(SAVED2->op,SAVED2->u,
	  SAVED4->next);			/* just Z on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(Z) */
    stk = dump1; dump1 = dump1->next->next->next;
    stk->next->next->next = SAVED5;
    POP(dump);
}

PRIVATE void unary4_()
{			/*  X Y Z W [P]  unary4    ==>  X' Y' Z' W'	*/
    FIVEPARAMS("unary4");
    ONEQUOTE("unary4");
    SAVESTACK;
    stk = SAVED4->next;				/* just X on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save p(X) */
    stk = newnode(SAVED4->op,SAVED4->u,
	  SAVED5->next);			/* just Y on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(Y) */
    stk = newnode(SAVED3->op,SAVED3->u,
	  SAVED5->next);			/* just Z on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(Z) */
    stk = newnode(SAVED2->op,SAVED2->u,
	  SAVED5->next);			/* just W on top */
    exeterm(SAVED1->u.lis);			/* execute P */
    dump1 = newnode(stk->op,stk->u,dump1);	/* save P(W) */
    stk = dump1; dump1 = dump1->next->next->next->next;
    stk->next->next->next->next = SAVED6;
    POP(dump);
}

PRIVATE void app12_()
{
    /*   X  Y  Z  [P]  app12  */
    THREEPARAMS("app12");
    unary2_();
    stk->next->next = stk->next->next->next;	/* delete X */
}

PRIVATE void map_()
{
    TWOPARAMS("map");
    ONEQUOTE("map");
    SAVESTACK;
    switch(SAVED2->op)
      { case LIST_:
	  { dump1 = newnode(LIST_,SAVED2->u,dump1);	/* step old */
	    dump2 = LIST_NEWNODE(0L,dump2);		/* head new */
	    dump3 = LIST_NEWNODE(0L,dump3);		/* last new */
	    while (DMP1 != NULL)
	      { stk = newnode(DMP1->op,
			      DMP1->u,SAVED3);
		exeterm(SAVED1->u.lis);
D(		printf("map: "); writefactor(stk, stdout); printf("\n"); )
		if (DMP2 == NULL)			/* first */
		  { DMP2 =
			newnode(stk->op,stk->u,NULL);
		    DMP3 = DMP2; }
		else					/* further */
		  { DMP3->next =
			newnode(stk->op,stk->u,NULL);
		    DMP3 = DMP3->next; }
		DMP1 = DMP1->next; }
	    stk = LIST_NEWNODE(DMP2,SAVED3);
	    POP(dump3);
	    POP(dump2);
	    POP(dump1);
	    break; }
	case STRING_:
	  { char *s, *resultstring; int j = 0;
	    resultstring =
		(char *) malloc(strlen(SAVED2->u.str) + 1);
	    for (s = SAVED2->u.str; *s != '\0'; s++)
	      { stk = CHAR_NEWNODE((long)*s,SAVED3);
		exeterm(SAVED1->u.lis);
		resultstring[j++] = stk->u.num; }
	    stk = STRING_NEWNODE(resultstring,SAVED3);
	    break; }
	case SET_:
	  { long i; long resultset = 0;
	    for (i = 0; i < SETSIZE; i++)
		if (SAVED2->u.set & (1 << i))
		  { stk = INTEGER_NEWNODE(i,SAVED3);
		    exeterm(SAVED1->u.lis);
		    resultset = resultset | (1 << stk->u.num); }
	    stk = SET_NEWNODE(resultset,SAVED3);
	    break; }
	default:
	    BADAGGREGATE("map"); }
    POP(dump);
}

PRIVATE void step_()
{
    TWOPARAMS("step");
    ONEQUOTE("step");
    SAVESTACK;
    stk = stk->next->next;
    switch(SAVED2->op)
      { case LIST_:
	  { dump1 = newnode(LIST_,SAVED2->u,dump1);
	    while (DMP1 != NULL)
	      { GNULLARY(DMP1->op,DMP1->u);
		exeterm(SAVED1->u.lis);
		DMP1 = DMP1->next; }
	    POP(dump1);
	    break; }
	case STRING_:
	  { char *s;
	    for (s = SAVED2->u.str; *s != '\0'; s++)
	      { stk = CHAR_NEWNODE((long)*s,stk);
		exeterm(SAVED1->u.lis); }
	    break; }
	case SET_:
	  { long i;
	    for (i = 0; i < SETSIZE; i++)
		if (SAVED2->u.set & (1 << i))
		  { stk = INTEGER_NEWNODE(i,stk);
		    exeterm(SAVED1->u.lis); }
	    break; }
	default:
	    BADAGGREGATE("step"); }
    POP(dump);
}

PRIVATE void fold_()
{
    THREEPARAMS("fold");
    swapd_();
    step_();
}

PRIVATE void cond_()
{
    int result = 0;
    ONEPARAM("cond");
				/* must check for QUOTES in list */
    LIST("cond");
    CHECKEMPTYLIST(stk->u.lis,"cond");
    SAVESTACK;
    dump1 = newnode(LIST_,stk->u,dump1);
    while ( result == 0 &&
	    DMP1 != NULL &&
	    DMP1->next != NULL )
      { stk = SAVED2;
	exeterm(DMP1->u.lis->u.lis);
	result = stk->u.num;
	if (!result) DMP1 = DMP1->next; }
    stk = SAVED2;
    if (result) exeterm(DMP1->u.lis->next);
	else exeterm(DMP1->u.lis); /* default */
    POP(dump1);
    POP(dump);
}

#define IF_TYPE(PROCEDURE,NAME,TYP)				\
    PRIVATE void PROCEDURE()					\
    {   TWOPARAMS(NAME);					\
	TWOQUOTES(NAME);					\
        SAVESTACK;						\
	stk = SAVED3;						\
	exeterm(stk->op == TYP ? SAVED2->u.lis : SAVED1->u.lis);\
	POP(dump); }

IF_TYPE(ifinteger_,"ifinteger",INTEGER_)
IF_TYPE(ifchar_,"ifchar",CHAR_)
IF_TYPE(iflogical_,"iflogical",BOOLEAN_)
IF_TYPE(ifstring_,"ifstring",STRING_)
IF_TYPE(ifset_,"ifset",SET_)
IF_TYPE(iffloat_,"iffloat",FLOAT_)
IF_TYPE(iffile_,"iffile",FILE_)
IF_TYPE(iflist_,"iflist",LIST_)

PRIVATE void filter_()
{
    TWOPARAMS("filter");
    ONEQUOTE("filter");
    SAVESTACK;
    switch (SAVED2->op)
      { case SET_ :
	  { long j; long resultset = 0;
	    for (j = 0; j < SETSIZE; j++)
	      { if (SAVED2->u.set & (1 << j))
		  { stk = INTEGER_NEWNODE(j,SAVED3);
		    exeterm(SAVED1->u.lis);
		    if (stk->u.num)
			resultset = resultset | (1 << j); } }
	    stk = SET_NEWNODE(resultset,SAVED3);
	    break; }
	case STRING_ :
	  { char *s, *resultstring; int j = 0;
	    resultstring =
		(char *) malloc(strlen(SAVED2->u.str) + 1);
	    for (s = SAVED2->u.str; *s != '\0'; s++)
	      { stk = CHAR_NEWNODE((long)*s, SAVED3);
		exeterm(SAVED1->u.lis);
		if (stk->u.num) resultstring[j++] = *s; }
	    resultstring[j] = '\0';
	    stk = STRING_NEWNODE(resultstring,SAVED3);
	    break; }
        case LIST_:
	  { dump1 = newnode(LIST_,SAVED2->u,dump1);	/* step old */
	    dump2 = LIST_NEWNODE(0L,dump2);		/* head new */
	    dump3 = LIST_NEWNODE(0L,dump3);		/* last new */
	    while (DMP1 != NULL)
	      { stk = newnode(DMP1->op,DMP1->u,SAVED3);
		exeterm(SAVED1->u.lis);
D(		printf("filter: "); writefactor(stk, stdout); printf("\n"); )
		if (stk->u.num)				/* test */
		    { if (DMP2 == NULL)			/* first */
		      { DMP2 =
			    newnode(DMP1->op,
				DMP1->u,NULL);
			DMP3 = DMP2; }
		    else				/* further */
		      { DMP3->next =
			    newnode(DMP1->op,
				DMP1->u,NULL);
			DMP3 = DMP3->next; } }
		DMP1 = DMP1->next; }
	    stk = LIST_NEWNODE(DMP2,SAVED3);
	    POP(dump3);
	    POP(dump2);
	    POP(dump1);
	    break; }
	default :
	    BADAGGREGATE("filter"); }
    POP(dump);
}

PRIVATE void split_()
{
    TWOPARAMS("split");
    SAVESTACK;
    switch (SAVED2->op)
      { case SET_ :
	  { long j; long yes_set = 0, no_set = 0;
	    for (j = 0; j < SETSIZE; j++)
	      { if (SAVED2->u.set & (1 << j))
		  { stk = INTEGER_NEWNODE(j,SAVED3);
		    exeterm(SAVED1->u.lis);
		    if (stk->u.num)
			  yes_set = yes_set | (1 << j);
		    else  no_set = no_set | (1 << j); } }
	    stk = SET_NEWNODE(yes_set,SAVED3);
	    NULLARY(SET_NEWNODE,no_set);
	    break; }
	case STRING_ :
	  { char *s, *yesstring, *nostring; int yesptr = 0, noptr = 0;
	    yesstring =
		(char *) malloc(strlen(SAVED2->u.str) + 1);
	    nostring =
		(char *) malloc(strlen(SAVED2->u.str) + 1);
	    for (s = SAVED2->u.str; *s != '\0'; s++)
	      { stk = CHAR_NEWNODE((long) *s, SAVED3);
		exeterm(SAVED1->u.lis);
		if (stk->u.num) yesstring[yesptr++] = *s;
			   else nostring[noptr++] = *s; }
	    yesstring[yesptr] = '\0'; nostring[noptr] = '\0';
	    stk = STRING_NEWNODE(yesstring,SAVED3);
	    NULLARY(STRING_NEWNODE,nostring);
	    break; }
        case LIST_:
	  { dump1 = newnode(LIST_,SAVED2->u,dump1);	/* step old */
	    dump2 = LIST_NEWNODE(0L,dump2);		/* head true */
	    dump3 = LIST_NEWNODE(0L,dump3);		/* last true */
	    dump4 = LIST_NEWNODE(0L,dump4);		/* head false */
	    dump5 = LIST_NEWNODE(0L,dump5);		/* last false */
	    while (DMP1 != NULL)
	      { stk = newnode(DMP1->op,DMP1->u,SAVED3);
		exeterm(SAVED1->u.lis);
D(		printf("split: "); writefactor(stk, stdout); printf("\n"); )
		if (stk->u.num)				/* pass */
		    if (DMP2 == NULL)			/* first */
		      { DMP2 =
			    newnode(DMP1->op,
				DMP1->u,NULL);
			DMP3 = DMP2; }
		    else				/* further */
		      { DMP3->next =
			    newnode(DMP1->op,
				DMP1->u,NULL);
			DMP3 = DMP3->next; }
		else					/* fail */
		    if (DMP4 == NULL)			/* first */
		      { DMP4 =
			    newnode(DMP1->op,
				DMP1->u,NULL);
			DMP5 = DMP4; }
		    else				/* further */
		      { DMP5->next =
			    newnode(DMP1->op,
				DMP1->u,NULL);
			DMP5 = DMP5->next; }
		DMP1 = DMP1->next; }
	    stk = LIST_NEWNODE(DMP2,SAVED3);
	    NULLARY(LIST_NEWNODE,DMP4);
	    POP(dump5);
	    POP(dump4);
	    POP(dump3);
	    POP(dump2);
	    POP(dump1);
	    break; }
	default :
	    BADAGGREGATE("split"); }
    POP(dump);
}

#define SOMEALL(PROCEDURE,NAME,INITIAL)				\
PRIVATE void PROCEDURE()					\
{   long result = INITIAL;					\
    TWOPARAMS(NAME);						\
    ONEQUOTE(NAME);						\
    SAVESTACK;							\
    switch (SAVED2->op)						\
      { case SET_ :						\
	  { long j;						\
	    for (j = 0; j < SETSIZE && result == INITIAL; j++)	\
	      { if (SAVED2->u.set & (1 << j))			\
		  { stk = INTEGER_NEWNODE(j,SAVED3);		\
		    exeterm(SAVED1->u.lis);			\
		    if (stk->u.num != INITIAL)			\
			result = 1 - INITIAL; } }		\
	    break; }						\
	case STRING_ :						\
	  { char *s;						\
	    for (s = SAVED2->u.str;				\
		 *s != '\0' && result == INITIAL; s++)		\
	      { stk = CHAR_NEWNODE((long)*s,SAVED3);		\
		exeterm(SAVED1->u.lis);				\
		if (stk->u.num != INITIAL)			\
		    result = 1 - INITIAL; }			\
	    break; }						\
	case LIST_ :						\
	  { dump1 = newnode(LIST_,SAVED2->u,dump1);		\
	    while (DMP1 != NULL && result == INITIAL)		\
	      { stk = newnode(DMP1->op,				\
			DMP1->u,SAVED3);			\
		exeterm(SAVED1->u.lis);				\
		if (stk->u.num != INITIAL)			\
		     result = 1 - INITIAL;			\
		DMP1 = DMP1->next; }				\
	    POP(dump1);						\
	    break; }						\
	default :						\
	    BADAGGREGATE(NAME); }				\
    stk = BOOLEAN_NEWNODE(result,SAVED3);			\
    POP(dump);							\
}
SOMEALL(some_,"some",0L)
SOMEALL(all_,"all",1L)

PRIVATE void primrec_()
{
    int n = 0; int i;
    THREEPARAMS("primrec");
    SAVESTACK;
    stk = stk->next->next->next;
    switch (SAVED3->op)
      { case LIST_:
	  { Node *current = SAVED3->u.lis;
	    while (current != NULL)
	      { stk = newnode(current->op,current->u,stk);
		current = current->next;
		n++; }
	    break; }
	case STRING_:
	  { char *s;
	    for (s = SAVED3->u.str; *s != '\0'; s++)
	      { stk = CHAR_NEWNODE((long) *s, stk);
		n++; }
	    break; }
	case SET_:
	  { long j; long set = SAVED3->u.set;
	    for (j = 0; j < SETSIZE; j++)
	        if (set & (1 << j))
		  { stk = INTEGER_NEWNODE(j,stk);
		    n++; }
	    break; }
	case INTEGER_:
	  { long j;
	    for (j = SAVED3->u.num; j > 0; j--)
	      { stk = INTEGER_NEWNODE(j, stk);
		n++; }
	    break; }
	default:
	    BADDATA("primrec"); }
    exeterm(SAVED2->u.lis);
    for (i = 1; i <= n; i++)
	exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void tailrecaux()
{
    int result;
    tailrec:
    dump1 = LIST_NEWNODE(stk,dump1);
    exeterm(SAVED3->u.lis);
    result = stk->u.num;
    stk = DMP1; POP(dump1);
    if (result) exeterm(SAVED2->u.lis); else
      { exeterm(SAVED1->u.lis);
	goto tailrec; }
}

PRIVATE void tailrec_()
{
    THREEPARAMS("tailrec");
    SAVESTACK;
    stk = SAVED4;
    tailrecaux();
    POP(dump);
}

PRIVATE void construct_()
{			/* [P] [[P1] [P2] ..] -> X1 X2 ..	*/
    TWOPARAMS("construct");
    TWOQUOTES("construct");
    SAVESTACK;
    stk = SAVED3;			/* pop progs		*/
    dump1 = LIST_NEWNODE(dump2,dump1);	/* save dump2		*/
    dump2 = stk;			/* save old stack	*/
    exeterm(SAVED2->u.lis);		/* [P]			*/
    dump3 = LIST_NEWNODE(stk,dump3);	/* save current stack	*/
    dump4 = newnode(LIST_,SAVED1->u,dump4);	/* step [..]	*/
    while (DMP4 != NULL)
      { stk = DMP3;			/* restore new stack	*/
	exeterm(DMP4->u.lis);
	dump2 = newnode(stk->op,stk->u,dump2); /* result	*/
	DMP4 = DMP4->next; }
    POP(dump4);
    POP(dump3);
    stk = dump2; dump2 = dump1->u.lis;	/* restore dump2	*/
    POP(dump1);
    POP(dump);
}

PRIVATE void branch_()
{
    THREEPARAMS("branch");
    TWOQUOTES("branch");
    SAVESTACK;
    stk = SAVED4;
    exeterm(SAVED3->u.num ? SAVED2->u.lis : SAVED1->u.lis);
    POP(dump);
}

PRIVATE void while_()
{
    TWOPARAMS("while");
    TWOQUOTES("while");
    SAVESTACK;
    do
      { stk = SAVED3;
	exeterm(SAVED2->u.lis);		/* TEST */
	if (! stk->u.num) break;
	stk = SAVED3;
	exeterm(SAVED1->u.lis);		/* DO */
	SAVED3 = stk; }
	while (1);
    stk = SAVED3;
    POP(dump);
}

PRIVATE void ifte_()
{
    int result;
    THREEPARAMS("ifte");
    THREEQUOTES("ifte");
    SAVESTACK;
    stk = SAVED4;
    exeterm(SAVED3->u.lis);
    result = stk->u.num;
    stk = SAVED4;
    exeterm(result ? SAVED2->u.lis : SAVED1->u.lis);
    POP(dump);
}

PRIVATE void condlinrecaux()
{
    int result = 0;
    dump1 = newnode(LIST_,SAVED1->u,dump1);
    dump2 = LIST_NEWNODE(stk,dump2);
    while ( result == 0 &&
	    DMP1 != NULL && DMP1->next != NULL )
      { stk = DMP2;
	exeterm(DMP1->u.lis->u.lis);
	result = stk->u.num;
	if (!result) DMP1 = DMP1->next; }
    stk = DMP2;
    if (result)
      { exeterm(DMP1->u.lis->next->u.lis);
	if (DMP1->u.lis->next->next != NULL)
	  { condlinrecaux();
	    exeterm(DMP1->u.lis->next->next->u.lis); } }
    else
      { exeterm(DMP1->u.lis->u.lis);
	if (DMP1->u.lis->next != NULL)
	  { condlinrecaux();
	    exeterm(DMP1->u.lis->next->u.lis); } }
    POP(dump2);
    POP(dump1);
}

PRIVATE void condlinrec_()
{
    ONEPARAM("condlinrec");
    LIST("condlinrec");
    CHECKEMPTYLIST(stk->u.lis,"condlinrec");
    SAVESTACK;
    stk = SAVED2;
    condlinrecaux();
    POP(dump);
}

PRIVATE void condnestrecaux()
{
    int result = 0;
    dump1 = newnode(LIST_,SAVED1->u,dump1);
    dump2 = LIST_NEWNODE(stk,dump2);
    while ( result == 0 &&
	    DMP1 != NULL && DMP1->next != NULL )
      { stk = DMP2;
	exeterm(DMP1->u.lis->u.lis);
	result = stk->u.num;
	if (!result) DMP1 = DMP1->next; }
    stk = DMP2;
    dump3 = LIST_NEWNODE(
		(result ? DMP1->u.lis->next : DMP1->u.lis),
		dump3 );
    exeterm(DMP3->u.lis);
    DMP3 = DMP3->next;
    while (DMP3 != NULL)
    { condnestrecaux();
      exeterm(DMP3->u.lis);
      DMP3 = DMP3->next; }
    POP(dump3);
/*
    if (result)
      { exeterm(DMP1->u.lis->next->u.lis);
	if (DMP1->u.lis->next->next != NULL)
	  { condnestrecaux();
	    exeterm(DMP1->u.lis->next->next->u.lis); } }
    else
      { exeterm(DMP1->u.lis->u.lis);
	if (DMP1->u.lis->next != NULL)
	  { condnestrecaux();
	    exeterm(DMP1->u.lis->next->u.lis); } }
*/
    POP(dump2);
    POP(dump1);
}

PRIVATE void condnestrec_()
{
    ONEPARAM("condnestrec");
    LIST("condnestrec");
    CHECKEMPTYLIST(stk->u.lis,"condnestrec");
    SAVESTACK;
    stk = SAVED2;
    condnestrecaux();
    POP(dump);
}

PRIVATE void linrecaux()
{
    int result;
    dump1 = LIST_NEWNODE(stk,dump1);
    exeterm(SAVED4->u.lis);
    result = stk->u.num;
    stk = DMP1; POP(dump1);
    if (result) exeterm(SAVED3->u.lis); else
      { exeterm(SAVED2->u.lis);
	linrecaux();
	exeterm(SAVED1->u.lis); }
}

PRIVATE void linrec_()
{
    FOURPARAMS("linrec");
    FOURQUOTES("linrec");
    SAVESTACK;
    stk = SAVED5;
    linrecaux();
    POP(dump);
}

PRIVATE void binrecaux()
{
    int result;
    dump1 = LIST_NEWNODE(stk,dump1);
    exeterm(SAVED4->u.lis);
    result = stk->u.num;
    stk = DMP1; POP(dump1);
    if (result) exeterm(SAVED3->u.lis); else
      { exeterm(SAVED2->u.lis);		/* split */
	dump2 = newnode(stk->op,stk->u,dump2);
	POP(stk);
	binrecaux();			/* first */
	GNULLARY(dump2->op,dump2->u);
	POP(dump2);
	binrecaux();			/* second */
	exeterm(SAVED1->u.lis); }	/* combine */
}

PRIVATE void binrec_()
{
    FOURPARAMS("binrec");
    FOURQUOTES("binrec");
    SAVESTACK;
    stk = SAVED5;
    binrecaux();
    POP(dump);
}

PRIVATE void treestepaux(item)
    Node *item;
{
    if (item->op != LIST_)
      { GNULLARY(item->op,item->u);
	exeterm(SAVED1->u.lis); }
    else
      { dump1 = newnode(LIST_,item->u,dump1);
	while (DMP1 != NULL)
	  { treestepaux(DMP1);
	    DMP1 = DMP1->next; }
	POP(dump1); }
}

PRIVATE void treestep_()
{
    TWOPARAMS("treestep");
    ONEQUOTE("treestep");
    SAVESTACK;
    stk = SAVED3;
    treestepaux(SAVED2);
    POP(dump);
}

PRIVATE void treerecaux()
{
    if (stk->next->op == LIST_)
      { NULLARY(LIST_NEWNODE,ANON_FUNCT_NEWNODE(treerecaux,NULL));
	cons_();		/*  D  [[[O] C] ANON_FUNCT_]	*/
D(	printf("treerecaux: stack = "); )
D(	writeterm(stk, stdout); printf("\n"); )
	exeterm(stk->u.lis->u.lis->next); }
    else
      { Node *n = stk;
	POP(stk);
	exeterm(n->u.lis->u.lis); }
}

PRIVATE void treerec_()
{
    THREEPARAMS("treerec");
    cons_();
D(  printf("deep: stack = "); writeterm(stk, stdout); printf("\n"); )
    treerecaux();
}

PRIVATE void genrecaux()
{
    int result;
D(  printf("genrecaux: stack = "); )
D(  writeterm(stk, stdout); printf("\n"); )
    SAVESTACK;
    POP(stk);
    exeterm(SAVED1->u.lis->u.lis);		/*	[I]	*/
    result = stk->u.num;
    stk = SAVED2;
    if (result)
	exeterm(SAVED1->u.lis->next->u.lis);	/*	[T]	*/
    else
      { exeterm(SAVED1->u.lis->next->next->u.lis); /*	[R1]	*/
	NULLARY(LIST_NEWNODE,SAVED1->u.lis);
	NULLARY(LIST_NEWNODE,ANON_FUNCT_NEWNODE(genrecaux,NULL));
	cons_();
	exeterm(SAVED1->u.lis->next->next->next); } /*   [R2]	*/
    POP(dump);
}

PRIVATE void genrec_()
{
    FOURPARAMS("genrec");
    FOURQUOTES("genrec");
    cons_(); cons_(); cons_();
    genrecaux();
}

PRIVATE void treegenrecaux()
{
D(  printf("treegenrecaux: stack = "); )
D(  writeterm(stk, stdout); printf("\n"); )
    if (stk->next->op == LIST_)
      { SAVESTACK;				/* begin DIP	*/
	POP(stk);
	exeterm(SAVED1->u.lis->next->u.lis);	/*	[O2]	*/
	GNULLARY(SAVED1->op,SAVED1->u);
	POP(dump);				/*   end DIP	*/
	NULLARY(LIST_NEWNODE,ANON_FUNCT_NEWNODE(treegenrecaux,NULL));
	cons_();
	exeterm(stk->u.lis->u.lis->next->next); } /*	[C]	*/
    else
      { Node *n = stk;
	POP(stk);
	exeterm(n->u.lis->u.lis); }		/*	[O1]	*/
}

PRIVATE void treegenrec_()
{					/* T [O1] [O2] [C]	*/
    FOURPARAMS("treegenrec");
    cons_(); cons_();
D(  printf("treegenrec: stack = "); writeterm(stk, stdout); printf("\n"); )
    treegenrecaux();
}

PRIVATE void plain_manual_()
{
    make_manual(0);
}

PRIVATE void html_manual_()
{
    make_manual(1);
}

PRIVATE void latex_manual_()
{
    make_manual(2);
}

PRIVATE void manual_list_aux_()
{
    manual_list_();
}

/* - - - - -   I N I T I A L I S A T I O N   - - - - - */

static struct {char *name; void (*proc) (); char *messg1, *messg2 ; }
    optable[] =
	/* THESE MUST BE DEFINED IN THE ORDER OF THEIR VALUES */
{

{"__ILLEGAL",		dummy_,		"->",
"internal error, cannot happen - supposedly."},

{"__COPIED",		dummy_,		"->",
"no message ever, used for gc."},

{"__USR",		dummy_,		"->",
"user node."},

{"__ANON_FUNCT",	dummy_,		"->",
"op for anonymous function call."},

/* LITERALS */

{" truth value type",	dummy_,		"->  B",
"The logical type, or the type of truth values.\nIt has just two literals: true and false."},

{" character type",	dummy_,		"->  C",
"The type of characters. Literals are written with a single quote.\nExamples:  'A  '7  ';  and so on. Unix style escapes are allowed."},

{" integer type",	dummy_,		"->  I",
"The type of negative, zero or positive integers.\nLiterals are written in decimal notation. Examples:  -123   0   42."},

{" set type",		dummy_,		"->  {...}",
"The type of sets of small non-negative integers.\nThe maximum is platform dependent, typically the range is 0..31.\nLiterals are written inside curly braces.\nExamples:  {}  {0}  {1 3 5}  {19 18 17}."},

{" string type",	dummy_,		"->  \"...\" ",
"The type of strings of characters. Literals are written inside double quotes.\nExamples: \"\"  \"A\"  \"hello world\" \"123\".\nUnix style escapes are accepted."},

{" list type",		dummy_,		"->  [...]",
"The type of lists of values of any type (including lists),\nor the type of quoted programs which may contain operators or combinators.\nLiterals of this type are written inside square brackets.\nExamples: []  [3 512 -7]  [john mary]  ['A 'C ['B]]  [dup *]."},

{" float type",		dummy_,		"->  F",
"The type of floating-point numbers.\nLiterals of this type are written with embedded decimal points (like 1.2)\nand optional exponent specifiers (like 1.5E2)"},

{" file type",		dummy_,		"->  FILE:",
"The type of references to open I/O streams,\ntypically but not necessarily files.\nThe only literals of this type are stdin, stdout, and stderr."},

/* OPERANDS */

{"false",		false_,		"->  false",
"Pushes the value false."},

{"true",		true_,		"->  true",
"Pushes the value true."},

{"maxint",		maxint_,	"->  maxint",
"Pushes largest integer (platform dependent). Typically it is 32 bits."},

{"setsize",		setsize_,	"->  setsize",
"Pushes the maximum number of elements in a set (platform dependent).\nTypically it is 32, and set members are in the range 0..31."},

{"stack",		stack_,		".. X Y Z  ->  .. X Y Z [Z Y X ..]",
"Pushes the stack as a list."},

{"__symtabmax",		symtabmax_,	"->  I",
"Pushes value of maximum size of the symbol table."},

{"__symtabindex",	symtabindex_,	"->  I",
"Pushes current size of the symbol table."},

{"__dump",		dump_,		"->  [..]",
"debugging only: pushes the dump as a list."},

{"conts",		conts_,		"->  [[P] [Q] ..]",
"Pushes current continuations. Buggy, do not use."},

{"autoput",		autoput_,	"->  I",
"Pushes current value of flag  for automatic output, I = 0..2."},

{"undeferror",		undeferror_,	"->  I",
"Pushes current value of undefined-is-error flag."},

{"undefs",		undefs_,	"->  [..]",
"Push a list of all undefined symbols in the current symbol table."},

{"echo",		echo_,		"->  I",
"Pushes value of echo flag, I = 0..3."},

{"clock",		clock_,		"->  I",
"Pushes the integer value of current CPU usage in hundreds of a second."},

{"time",		time_,		"->  I",
"Pushes the current time (in seconds since the Epoch)."},

{"rand",		rand_,		"->  I",
"I is a random integer."},

{"__memorymax",		memorymax_,	"->  I",
"Pushes value of total size of memory."},

{"stdin",		stdin_,		"->  S",
"Pushes the standard input stream."},

{"stdout",		stdout_,	"->  S",
"Pushes the standard output stream."},

{"stderr",		stderr_,	"->  S",
"Pushes the standard error stream."},

/* OPERATORS */

{"id",			id_,		"->",
"Identity function, does nothing.\nAny program of the form  P id Q  is equivalent to just  P Q."},

{"dup",			dup_,		"X  ->  X X",
"Pushes an extra copy of X onto stack."},

{"swap",		swap_,		"X Y  ->  Y X",
"Interchanges X and Y on top of the stack."},

{"rollup",              rollup_,        "X Y Z  ->  Z X Y",
"Moves X and Y up, moves Z down"},

{"rolldown",            rolldown_,      "X Y Z  ->  Y Z X",
"Moves Y and Z down, moves X up"},

{"rotate",              rotate_,        "X Y Z  ->  Z Y X",
"Interchanges X and Z"},

{"popd",                popd_,          "Y Z  ->  Z",
"As if defined by:   popd  ==  [pop] dip "},

{"dupd",                dupd_,          "Y Z  ->  Y Y Z",
"As if defined by:   dupd  ==  [dup] dip"},

{"swapd",               swapd_,         "X Y Z  ->  Y X Z",
"As if defined by:   swapd  ==  [swap] dip"},

{"rollupd",             rollupd_,       "X Y Z W  ->  Z X Y W",
"As if defined by:   rollupd  ==  [rollup] dip"},

{"rolldownd",           rolldownd_,     "X Y Z W  ->  Y Z X W",
"As if defined by:   rolldownd  ==  [rolldown] dip "},

{"rotated",             rotated_,       "X Y Z W  ->  Z Y X W",
"As if defined by:   rotated  ==  [rotate] dip"},

{"pop",			pop_,		"X  ->",
"Removes X from top of the stack."},

{"choice",		choice_,	"B T F  ->  X",
"If B is true, then X = T else X = F."},

{"or",			or_,		"X Y  ->  Z",
"Z is the union of sets X and Y, logical disjunction for truth values."},

{"xor",			xor_,		"X Y  ->  Z",
"Z is the symmetric difference of sets X and Y,\nlogical exclusive disjunction for truth values."},

{"and",			and_,		"X Y  ->  Z",
"Z is the intersection of sets X and Y, logical conjunction for truth values."},

{"not",			not_,		"X  ->  Y",
"Y is the complement of set X, logical negation for truth values."},

{"+",			plus_,		"M I  ->  N",
"Numeric N is the result of adding integer I to numeric M.\nAlso supports float."},

{"-",			minus_,		"M I  ->  N",
"Numeric N is the result of subtracting integer I from numeric M.\nAlso supports float."},

{"*",			mul_,		"I J  ->  K",
"Integer K is the product of integers I and J.  Also supports float."},

{"/",			divide_,	"I J  ->  K",
"Integer K is the (rounded) ratio of integers I and J.  Also supports float."},

{"rem",			rem_,		"I J  ->  K",
"Integer K is the remainder of dividing I by J.  Also supports float."},

{"div",			div_,		"I J  ->  K L",
"Integers K and L are the quotient and remainder of dividing I by J."},

{"sign",		sign_,		"N1  ->  N2",
"Integer N2 is the sign (-1 or 0 or +1) of integer N1,\nor float N2 is the sign (-1.0 or 0.0 or 1.0) of float N1."},

{"neg",			neg_,		"I  ->  J",
"Integer J is the negative of integer I.  Also supports float."},

{"ord",			ord_,		"C  ->  I",
"Integer I is the Ascii value of character C (or logical or integer)."},

{"chr",			chr_,		"I  ->  C",
"C is the character whose Ascii value is integer I (or logical or character)."},

{"abs",			abs_,		"N1  ->  N2",
"Integer N2 is the absolute value (0,1,2..) of integer N1,\nor float N2 is the absolute value (0.0 ..) of float N1"},

{"acos",		acos_,		"F  ->  G",
"G is the arc cosine of F."},

{"asin",		asin_,		"F  ->  G",
"G is the arc sine of F."},

{"atan",		atan_,		"F  ->  G",
"G is the arc tangent of F."},

{"atan2",		atan2_,		"F G  ->  H",
"H is the arc tangent of F / G."},

{"ceil",		ceil_,		"F  ->  G",
"G is the float ceiling of F."},

{"cos",			cos_,		"F  ->  G",
"G is the cosine of F."},

{"cosh",		cosh_,		"F  ->  G",
"G is the hyperbolic cosine of F."},

{"exp",			exp_,		"F  ->  G",
"G is e (2.718281828...) raised to the Fth power."},

{"floor",		floor_,		"F  ->  G",
"G is the floor of F."},

{"frexp",		frexp_,		"F  ->  G I",
"G is the mantissa and I is the exponent of F.\nUnless F = 0, 0.5 <= abs(G) < 1.0."},

{"ldexp",		ldexp_,		"F I  -> G",
"G is F times 2 to the Ith power."},

{"log",			log_,		"F  ->  G",
"G is the natural logarithm of F."},

{"log10",		log10_,		"F  ->  G",
"G is the common logarithm of F."},

{"modf",		modf_,		"F  ->  G H",
"G is the fractional part and H is the integer part\n(but expressed as a float) of F."},

{"pow",			pow_,		"F G  ->  H",
"H is F raised to the Gth power."},

{"sin",			sin_,		"F  ->  G",
"G is the sine of F."},

{"sinh",		sinh_,		"F  ->  G",
"G is the hyperbolic sine of F."},

{"sqrt",		sqrt_,		"F  ->  G",
"G is the square root of F."},

{"tan",			tan_,		"F  ->  G",
"G is the tangent of F."},

{"tanh",		tanh_,		"F  ->  G",
"G is the hyperbolic tangent of F."},

{"trunc",		trunc_,		"F  ->  I",
"I is an integer equal to the float F truncated toward zero."},

{"localtime",		localtime_,	"I  ->  T",
"Converts a time I into a list T representing local time:\n[year month day hour minute second isdst yearday weekday].\nMonth is 1 = January ... 12 = December;\nisdst is a Boolean flagging daylight savings/summer time;\nweekday is 0 = Monday ... 7 = Sunday."},

{"gmtime",		gmtime_,	"I  ->  T",
"Converts a time I into a list T representing universal time:\n[year month day hour minute second isdst yearday weekday].\nMonth is 1 = January ... 12 = December;\nisdst is false; weekday is 0 = Monday ... 7 = Sunday."},

{"mktime",		mktime_,	"T  ->  I",
"Converts a list T representing local time into a time I.\nT is in the format generated by localtime."},

{"strftime",		strftime_,	"T S1  ->  S2",
"Formats a list T in the format of localtime or gmtime\nusing string S1 and pushes the result S2."},

{"strtol",		strtol_,	"S I  ->  J",
"String S is converted to the integer J using base I.\nIf I = 0, assumes base 10,\nbut leading \"0\" means base 8 and leading \"0x\" means base 16."},

{"strtod",		strtod_,	"S  ->  R",
"String S is converted to the float R."},

{"format",		format_,	"N C I J  ->  S",
"S is the formatted version of N in mode C\n('d or 'i = decimal, 'o = octal, 'x or\n'X = hex with lower or upper case letters)\nwith maximum width I and minimum width J."},

{"formatf",		formatf_,	"F C I J  ->  S",
"S is the formatted version of F in mode C\n('e or 'E = exponential, 'f = fractional,\n'g or G = general with lower or upper case letters)\nwith maximum width I and precision J."},

{"srand",		srand_,		"I  ->  ",
"Sets the random integer seed to integer I."},

{"pred",		pred_,		"M  ->  N",
"Numeric N is the predecessor of numeric M."},

{"succ",		succ_,		"M  ->  N",
"Numeric N is the successor of numeric M."},

{"max",			max_,		"N1 N2  ->  N",
"N is the maximum of numeric values N1 and N2.  Also supports float."},

{"min",			min_,		"N1 N2  ->  N",
"N is the minimum of numeric values N1 and N2.  Also supports float."},

{"fclose",		fclose_,	"S  ->  ",
"Stream S is closed and removed from the stack."},

{"feof",		feof_,		"S  ->  S B",
"B is the end-of-file status of stream S."},

{"ferror",		ferror_,	"S  ->  S B",
"B is the error status of stream S."},

{"fflush",		fflush_,	"S  ->  S",
"Flush stream S, forcing all buffered output to be written."},

{"fgetch",		fgetch_,	"S  ->  S C",
"C is the next available character from stream S."},

{"fgets",		fgets_,		"S  ->  S L",
"L is the next available line (as a string) from stream S."},

{"fopen",		fopen_,		"P M  ->  S",
"The file system object with pathname P is opened with mode M (r, w, a, etc.)\nand stream object S is pushed; if the open fails, file:NULL is pushed."},

{"fread",		fread_,		"S I  ->  S L",
"I bytes are read from the current position of stream S\nand returned as a list of I integers."},

{"fwrite",		fwrite_,	"S L  ->  S",
"A list of integers are written as bytes to the current position of stream S."},

{"fremove",		fremove_,	"P  ->  B",
"The file system object with pathname P is removed from the file system.\n is a boolean indicating success or failure."},

{"frename",		frename_,	"P1 P2  ->  B",
"The file system object with pathname P1 is renamed to P2.\nB is a boolean indicating success or failure."},

{"fput",		fput_,		"S X  ->  S",
"Writes X to stream S, pops X off stack."},

{"fputch",		fputch_,	"S C  ->  S",
"The character C is written to the current position of stream S."},

{"fputchars",		fputchars_,	"S \"abc..\"  ->  S",
"The string abc.. (no quotes) is written to the current position of stream S."},

{"fputstring",		fputchars_,	"S \"abc..\"  ->  S",
"== fputchars, as a temporary alternative."},

{"fseek",		fseek_,		"S P W  ->  S",
"Stream S is repositioned to position P relative to whence-point W,\nwhere W = 0, 1, 2 for beginning, current position, end respectively."},

{"ftell",		ftell_,		"S  ->  S I",
"I is the current position of stream S."},

{"unstack",		unstack_,	"[X Y ..]  ->  ..Y X",
"The list [X Y ..] becomes the new stack."},

{"cons",		cons_,		"X A  ->  B",
"Aggregate B is A with a new member X (first member for sequences)."},

{"swons",		swons_,		"A X  ->  B",
"Aggregate B is A with a new member X (first member for sequences)."},

{"first",		first_,		"A  ->  F",
"F is the first member of the non-empty aggregate A."},

{"rest",		rest_,		"A  ->  R",
"R is the non-empty aggregate A with its first member removed."},

{"compare",		compare_,	"A B  ->  I",
"I (=-1,0,+1) is the comparison of aggregates A and B.\nThe values correspond to the predicates <=, =, >=."},

{"at",			at_,		"A I  ->  X",
"X (= A[I]) is the member of A at position I."},

{"of",			of_,		"I A  ->  X",
"X (= A[I]) is the I-th member of aggregate A."},

{"size",		size_,		"A  ->  I",
"Integer I is the number of elements of aggregate A."},

{"opcase",		opcase_,	"X [..[X Xs]..]  ->  [Xs]",
"Indexing on type of X, returns the list [Xs]."},

{"case",		case_,		"X [..[X Y]..]  ->  Y i",
"Indexing on the value of X, execute the matching Y."},

{"uncons",		uncons_,	"A  ->  F R",
"F and R are the first and the rest of non-empty aggregate A."},

{"unswons",		unswons_,	"A  ->  R F",
"R and F are the rest and the first of non-empty aggregate A."},

{"drop",		drop_,		"A N  ->  B",
"Aggregate B is the result of deleting the first N elements of A."},

{"take",		take_,		"A N  ->  B",
"Aggregate B is the result of retaining just the first N elements of A."},

{"concat",		concat_,	"S T  ->  U",
"Sequence U is the concatenation of sequences S and T."},

{"enconcat",		enconcat_,	"X S T  ->  U",
"Sequence U is the concatenation of sequences S and T\nwith X inserted between S and T (== swapd cons concat)"},

{"name",		name_,		"sym  ->  \"sym\"",
"For operators and combinators, the string \"sym\" is the name of item sym,\nfor literals sym the result string is its type."},

{"intern",		intern_,	"\"sym\"  -> sym",
"Pushes the item whose name is \"sym\"."},

{"body",		body_,		"U  ->  [P]",
"Quotation [P] is the body of user-defined symbol U."},

/* PREDICATES */

{"null",		null_,		"X  ->  B",
"Tests for empty aggregate X or zero numeric."},

{"small",		small_,		"X  ->  B",
"Tests whether aggregate X has 0 or 1 members, or numeric 0 or 1."},

{">=",			geql_,		"X Y  ->  B",
"Either both X and Y are numeric or both are strings or symbols.\nTests whether X greater than or equal to Y.  Also supports float."},

{">",			greater_,	"X Y  ->  B",
"Either both X and Y are numeric or both are strings or symbols.\nTests whether X greater than Y.  Also supports float."},

{"<=",			leql_,		"X Y  ->  B",
"Either both X and Y are numeric or both are strings or symbols.\nTests whether X less than or equal to Y.  Also supports float."},

{"<",			less_,		"X Y  ->  B",
"Either both X and Y are numeric or both are strings or symbols.\nTests whether X less than Y.  Also supports float."},

{"!=",			neql_,		"X Y  ->  B",
"Either both X and Y are numeric or both are strings or symbols.\nTests whether X not equal to Y.  Also supports float."},

{"=",			eql_,		"X Y  ->  B",
"Either both X and Y are numeric or both are strings or symbols.\nTests whether X equal to Y.  Also supports float."},

{"equal",		equal_,		"T U  ->  B",
"(Recursively) tests whether trees T and U are identical."},

{"has",			has_,		"A X  ->  B",
"Tests whether aggregate A has X as a member."},

{"in",			in_,		"X A  ->  B",
"Tests whether X is a member of aggregate A."},

{"integer",		integer_,	"X  ->  B",
"Tests whether X is an integer."},

{"char",		char_,		"X  ->  B",
"Tests whether X is a character."},

{"logical",		logical_,	"X  ->  B",
"Tests whether X is a logical."},

{"set",			set_,		"X  ->  B",
"Tests whether X is a set."},

{"string",		string_,	"X  ->  B",
"Tests whether X is a string."},

{"list",		list_,		"X  ->  B",
"Tests whether X is a list."},

{"leaf",		leaf_,		"X  ->  B",
"Tests whether X is not a list."},

{"user",		user_,		"X  ->  B",
"Tests whether X is a user-defined symbol."},

{"float",		float_,		"R  ->  B",
"Tests whether R is a float."},

{"file",		file_,		"F  ->  B",
"Tests whether F is a file."},

/* COMBINATORS */

{"i",			i_,		"[P]  ->  ...",
"Executes P. So, [P] i  ==  P."},

{"x",			x_,		"[P]i  ->  ...",
"Executes P without popping [P]. So, [P] x  ==  [P] P."},

{"dip",			dip_,		"X [P]  ->  ... X",
"Saves X, executes P, pushes X back."},

{"app1",		app1_,		"X [P]  ->  R",
"Executes P, pushes result R on stack without X."},

{"app11",		app11_,		"X Y [P]  ->  R",
"Executes P, pushes result R on stack."},

{"app12",		app12_,		"X Y1 Y2 [P]  ->  R1 R2",
"Executes P twice, with Y1 and Y2, returns R1 and R2."},

{"construct",		construct_,	"[P] [[P1] [P2] ..]  ->  R1 R2 ..",
"Saves state of stack and then executes [P].\nThen executes each [Pi] to give Ri pushed onto saved stack."},

{"nullary",		nullary_,	"[P]  ->  R",
"Executes P, which leaves R on top of the stack.\nNo matter how many parameters this consumes, none are removed from the stack."},

{"unary",		unary_,		"X [P]  ->  R",
"Executes P, which leaves R on top of the stack.\nNo matter how many parameters this consumes,\nexactly one is removed from the stack."},

{"unary2",		unary2_,	"X1 X2 [P]  ->  R1 R2",
"Executes P twice, with X1 and X2 on top of the stack.\nReturns the two values R1 and R2."},

{"unary3",		unary3_,	"X1 X2 X3 [P]  ->  R1 R2 R3",
"Executes P three times, with Xi, returns Ri (i = 1..3)."},

{"unary4",		unary4_,	"X1 X2 X3 X4 [P]  ->  R1 R2 R3 R4",
"Executes P four times, with Xi, returns Ri (i = 1..4)."},

{"app2",		unary2_,	"X1 X2 [P]  ->  R1 R2",
"Obsolescent.  == unary2"},

{"app3",		unary3_,	"X1 X2 X3 [P]  ->  R1 R2 R3",
"Obsolescent.  == unary3"},

{"app4",		unary4_,	"X1 X2 X3 X4 [P]  ->  R1 R2 R3 R4",
"Obsolescent.  == unary4"},

{"binary",		binary_,	"X Y [P]  ->  R",
"Executes P, which leaves R on top of the stack.\nNo matter how many parameters this consumes,\nexactly two are removed from the stack."},

{"ternary",		ternary_,	"X Y Z [P]  ->  R",
"Executes P, which leaves R on top of the stack.\nNo matter how many parameters this consumes,\nexactly three are removed from the stack."},

{"cleave",		cleave_,	"X [P1] [P2]  ->  R1 R2",
"Executes P1 and P2, each with X on top, producing two results."},

{"branch",		branch_,	"B [T] [F]  ->  ...",
"If B is true, then executes T else executes F."},

{"ifte",		ifte_,		"[B] [T] [F]  ->  ...",
"Executes B. If that yields true, then executes T else executes F."},

{"ifinteger",		ifinteger_,	"X [T] [E]  ->  ...",
"If X is an integer, executes T else executes E."},

{"ifchar",		ifchar_,	"X [T] [E]  ->  ...",
"If X is a character, executes T else executes E."},

{"iflogical",		iflogical_,	"X [T] [E]  ->  ...",
"If X is a logical or truth value, executes T else executes E."},

{"ifset",		ifset_,		"X [T] [E]  ->  ...",
"If X is a set, executes T else executes E."},

{"ifstring",		ifstring_,	"X [T] [E]  ->  ...",
"If X is a string, executes T else executes E."},

{"iflist",		iflist_,	"X [T] [E]  ->  ...",
"If X is a list, executes T else executes E."},

{"iffloat",		iffloat_,	"X [T] [E]  ->  ...",
"If X is a float, executes T else executes E."},

{"iffile",		iffile_,	"X [T] [E]  ->  ...",
"If X is a file, executes T else executes E."},

{"cond",		cond_,		"[..[[Bi] Ti]..[D]]  ->  ...",
"Tries each Bi. If that yields true, then executes Ti and exits.\nIf no Bi yields true, executes default D."},

{"while",		while_,		"[B] [D]  ->  ...",
"While executing B yields true executes D."},

{"linrec",		linrec_,	"[P] [T] [R1] [R2]  ->  ...",
"Executes P. If that yields true, executes T.\nElse executes R1, recurses, executes R2."},

{"tailrec",		tailrec_,	"[P] [T] [R1]  ->  ...",
"Executes P. If that yields true, executes T.\nElse executes R1, recurses."},

{"binrec",		binrec_,	"[B] [T] [R1] [R2]  ->  ...",
"Executes P. If that yields true, executes T.\nElse uses R1 to produce two intermediates, recurses on both,\nthen executes R2 to combines their results."},

{"genrec",		genrec_,	"[B] [T] [R1] [R2]  ->  ...",
"Executes B, if that yields true executes T.\nElse executes R1 and then [[B] [T] [R1] [R2] genrec] R2."},

{"condnestrec",		condnestrec_,	"[ [C1] [C2] .. [D] ]  ->  ...",
"A generalisation of condlinrec. Each [Ci] is of the form [[B] [R1] [R2] .. [Rn]] and [D] is of the form [[R1] [R2] .. [Rn]]. Tries each B, or if all fail, takes the default [D]. For the case taken, executes each [Ri] but recurses between any two consecutive [Ri]. (n > 3 would be exceptional.)"},

{"condlinrec",		condlinrec_,	"[ [C1] [C2] .. [D] ]  ->  ...",
"Each [Ci] is of the forms [[B] [T]] or [[B] [R1] [R2]].\nTries each B. If that yields true and there is just a [T], executes T and exit.\nIf there are [R1] and [R2], executes R1, recurses, executes R2.\nSubsequent cases are ignored. If no B yields true, then [D] is used.\nIt is then of the forms [[T]] or [[R1] [R2]]. For the former, executes T.\nFor the latter executes R1, recurses, executes R2."},

{"step",		step_,		"A  [P]  ->  ...",
"Sequentially putting members of aggregate A onto stack,\nexecutes P for each member of A."},

{"fold",		fold_,		"A V0 [P]  ->  V",
"Starting with value V0, sequentially pushes members of aggregate A\nand combines with binary operator P to produce value V."},

{"map",			map_,		"A [P]  ->  B",
"Executes P on each member of aggregate A,\ncollects results in sametype aggregate B."},

{"times",		times_,		"N [P]  ->  ...",
"N times executes P."},

{"infra",		infra_,		"L1 [P]  ->  L2",
"Using list L1 as stack, executes P and returns a new list L2.\nThe first element of L1 is used as the top of stack,\nand after execution of P the top of stack becomes the first element of L2."},

{"primrec",		primrec_,	"X [I] [C]  ->  R",
"Executes I to obtain an initial value R0.\nFor integer X uses increasing positive integers to X, combines by C for new R.\nFor aggregate X uses successive members and combines by C for new R."},

{"filter",		filter_,	"A [B]  ->  A1",
"Uses test B to filter aggregate A producing sametype aggregate A1."},

{"split",		split_,		"A [B]  ->  A1 A2",
"Uses test B to split aggregate A into sametype aggregates A1 and A2 ."},

{"some",		some_,		"A  [B]  ->  X",
"Applies test B to members of aggregate A, X = true if some pass."},

{"all",			all_,		"A [B]  ->  X",
"Applies test B to members of aggregate A, X = true if all pass."},

{"treestep",		treestep_,	"T [P]  ->  ...",
"Recursively traverses leaves of tree T, executes P for each leaf."},

{"treerec",		treerec_,	"T [O] [C]  ->  ...",
"T is a tree. If T is a leaf, executes O. Else executes [[O] [C] treerec] C."},

{"treegenrec",		treegenrec_,	"T [O1] [O2] [C]  ->  ...",
"T is a tree. If T is a leaf, executes O1.\nElse executes O2 and then [[O1] [O2] [C] treegenrec] C."},

/* MISCELLANEOUS */

{"help",		help1_,		"->",
"Lists all defined symbols, including those from library files.\nThen lists all primitives of raw Joy\n(There is a variant: \"_help\" which lists hidden symbols)."},

{"_help",		h_help1_,	"->",
"Lists all hidden symbols in library and then all hidden inbuilt symbols."},

{"helpdetail",		helpdetail_,	"[ S1  S2  .. ]",
"Gives brief help on each symbol S in the list."},

{"manual",		plain_manual_,	"->",
"Writes this manual of all Joy primitives to output file."},

{"__html_manual",	html_manual_,	"->",
"Writes this manual of all Joy primitives to output file in HTML style."},

{"__latex_manual",	latex_manual_,	"->",
"Writes this manual of all Joy primitives to output file in Latex style but without the head and tail."},

{"__manual_list",	manual_list_aux_, "->  L",
"Pushes a list L of lists (one per operator) of three documentation strings"},

{"__settracegc",	settracegc_,	"I  ->",
"Sets value of flag for tracing garbage collection to I (= 0..5)."},

{"setautoput",		setautoput_,	"I  ->",
"Sets value of flag for automatic put to I (if I = 0, none;\nif I = 1, put; if I = 2, stack)."},

{"setundeferror",	setundeferror_,	"I  ->",
"Sets flag that controls behavior of undefined functions\n(0 = no error, 1 = error)."},

{"setecho",		setecho_,	"I ->",
"Sets value of echo flag for listing.\nI = 0: no echo, 1: echo, 2: with tab, 3: and linenumber."},

{"gc",			gc_,		"->",
"Initiates garbage collection."},

{"system",		system_,	"\"command\"  ->",
"Escapes to shell, executes string \"command\".\nThe string may cause execution of another program.\nWhen that has finished, the process returns to Joy."},

{"getenv",		getenv_,	"\"variable\"  ->  \"value\"",
"Retrieves the value of the environment variable \"variable\"."},

{"argv",		argv_,		"-> A",
"Creates an aggregate A containing the interpreter's command line arguments."},

{"argc",		argc_,		"-> I",
"Pushes the number of command line arguments. This is equivalent to 'argv size'."},

{"__memoryindex",	memoryindex_,	"->",
"Pushes current value of memory."},

{"get",			get_,		"->  F",
"Reads a factor from input and pushes it onto stack."},

{"put",			put_,		"X  ->",
"Writes X to output, pops X off stack."},

{"putch",		putch_,		"N  ->",
"N : numeric, writes character whose ASCII is N."},

{"putchars",		putchars_,	"\"abc..\"  ->",
"Writes  abc.. (without quotes)"},

{"include",		include_,	"\"filnam.ext\"  ->",
"Transfers input to file whose name is \"filnam.ext\".\nOn end-of-file returns to previous input file."},

{"abort",		abortexecution_, "->",
"Aborts execution of current Joy program, returns to Joy main cycle."},

{"quit",		quit_,		"->",
"Exit from Joy."},

{0, dummy_, "->","->"}
};

PUBLIC void inisymboltable(void)		/* initialise		*/
{
    int i; char *s;
    symtabindex = symtab;
    for (i = 0; i < HASHSIZE; hashentry[i++] = symtab) ;
    localentry = symtab;
    for (i = 0; optable[i].name; i++)
      { s = optable[i].name;
	/* ensure same algorithm in getsym */
	for (hashvalue = 0; *s != '\0';) hashvalue += *s++;
	hashvalue %= HASHSIZE;
	symtabindex->name = optable[i].name;
	symtabindex->u.proc = optable[i].proc;
	symtabindex->next = hashentry[hashvalue];
	hashentry[hashvalue] = symtabindex;
D(	printf("entered %s in symbol table at %ld = %ld\n", \
	    symtabindex->name, (long)symtabindex, \
	    LOC2INT(symtabindex)); )
	symtabindex++; }
    firstlibra = symtabindex;
}

PRIVATE void helpdetail_()
{
    Node *n;
    ONEPARAM("HELP");
    LIST("HELP");
    printf("\n");
    n = stk->u.lis;
    while (n != NULL)
      { if (n->op == USR_)
	  { printf("%s  ==\n    ",n->u.ent->name);
	    writeterm(n->u.ent->u.body, stdout);
	    printf("\n"); break; }
	else
	    printf("%s        :   %s.\n%s\n",
		optable[ (int) n->op].name,
		optable[ (int) n->op].messg1,
		optable[ (int) n->op].messg2);
	printf("\n");
	n = n->next; }
    POP(stk);
}
#define PLAIN (style == 0)
#define HTML (style == 1)
#define LATEX (style == 2)
#define HEADER(N,NAME,HEAD)					\
    if (strcmp(N,NAME) == 0)					\
      { printf("\n\n");						\
        if (LATEX) printf("\\item[--- \\BX{");			\
	printf("%s",HEAD);					\
	if (LATEX) printf("} ---] \\verb# #");			\
	printf("\n\n"); }

PRIVATE void make_manual(int style /* 0=plain, 1=HTML, 2=Latex */)
{
    int i; char *n;
    if (HTML) printf("<HTML>\n<DL>\n");
    for (i = BOOLEAN_; optable[i].name != 0; i++)
      { n = optable[i].name;
	HEADER(n," truth value type","literal") else
	HEADER(n,"false","operand") else
	HEADER(n,"id","operator") else
	HEADER(n,"null","predicate") else
	HEADER(n,"i","combinator") else
	HEADER(n,"help","miscellaneous commands")
	if (n[0] != '_')
	  { if (HTML) printf("\n<DT>");
	    else if (LATEX)
	      { if (n[0] == ' ')
		  { n++;  printf("\\item[\\BX{"); }
		else printf("\\item[\\JX{"); }
	    if (HTML && strcmp(n,"<=")==0)
		printf("&lt;=");
	    else printf("%s",n);
	    if (LATEX) printf("}]  \\verb#");
	    if (HTML) printf(" <CODE>      :  </CODE> ");
		/* the above line does not produce the spaces around ":" */
	    else printf("\t:  ");
	    printf("%s", optable[i].messg1);
	    if (HTML) printf("\n<DD>");
	    else if (LATEX) printf("# \\\\ \n {\\small\\verb#");
		else printf("\n");
	    printf("%s", optable[i].messg2);
	    if (LATEX) printf("#}");
	    printf("\n\n"); } }
    if (HTML) printf("\n</DL>\n</HTML>\n");
}

PRIVATE void manual_list_()
{
    int i = -1;
    Node *tmp;
    Node *n = NULL;
    while (optable[++i].name) ; /* find end */
    --i; /* overshot */
    while (i)
      { tmp = STRING_NEWNODE(optable[i].messg2, NULL);
	tmp = STRING_NEWNODE(optable[i].messg1, tmp);
	tmp = STRING_NEWNODE(optable[i].name, tmp);
	n = LIST_NEWNODE(tmp, n);
	--i; }
    stk = LIST_NEWNODE(n,stk);
}

PUBLIC char *opername(int o)
{
    return optable[(short)o].name;
}
/* END of INTERP.C */
