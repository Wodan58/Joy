/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.30.1.8
 *  date    : 01/08/21
 */

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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "globals.h"
#ifdef CORRECT_INTERN_LOOKUP
#include <ctype.h>
#endif

void my_srand(unsigned num);
int my_rand(void);

PRIVATE void helpdetail_(void); /* this file		*/
PRIVATE void undefs_(void);
PRIVATE void make_manual(int style /* 0=plain, 1=html, 2=latex */);
PRIVATE void manual_list_(void);
#if 0
PRIVATE void manual_list_aux_(void);
#endif

#ifdef RUNTIME_CHECKS
#define ONEPARAM(NAME)                                                         \
    if (stck == NULL)                                                          \
    execerror("one parameter", NAME)
#define TWOPARAMS(NAME)                                                        \
    if (stck == NULL || stck->next == NULL)                                    \
    execerror("two parameters", NAME)
#define THREEPARAMS(NAME)                                                      \
    if (stck == NULL || stck->next == NULL || stck->next->next == NULL)        \
    execerror("three parameters", NAME)
#define FOURPARAMS(NAME)                                                       \
    if (stck == NULL || stck->next == NULL || stck->next->next == NULL         \
        || stck->next->next->next == NULL)                                     \
    execerror("four parameters", NAME)
#define FIVEPARAMS(NAME)                                                       \
    if (stck == NULL || stck->next == NULL || stck->next->next == NULL         \
        || stck->next->next->next == NULL                                      \
        || stck->next->next->next->next == NULL)                               \
    execerror("five parameters", NAME)
#define ONEQUOTE(NAME)                                                         \
    if (stck->op != LIST_)                                                     \
    execerror("quotation as top parameter", NAME)
#define TWOQUOTES(NAME)                                                        \
    ONEQUOTE(NAME);                                                            \
    if (stck->next->op != LIST_)                                               \
    execerror("quotation as second parameter", NAME)
#define THREEQUOTES(NAME)                                                      \
    TWOQUOTES(NAME);                                                           \
    if (stck->next->next->op != LIST_)                                         \
    execerror("quotation as third parameter", NAME)
#define FOURQUOTES(NAME)                                                       \
    THREEQUOTES(NAME);                                                         \
    if (stck->next->next->next->op != LIST_)                                   \
    execerror("quotation as fourth parameter", NAME)
#define SAME2TYPES(NAME)                                                       \
    if (stck->op != stck->next->op)                                            \
    execerror("two parameters of the same type", NAME)
#define STRING(NAME)                                                           \
    if (stck->op != STRING_)                                                   \
    execerror("string", NAME)
#define STRING2(NAME)                                                          \
    if (stck->next->op != STRING_)                                             \
    execerror("string as second parameter", NAME)
#define INTEGER(NAME)                                                          \
    if (stck->op != INTEGER_)                                                  \
    execerror("integer", NAME)
#define INTEGER2(NAME)                                                         \
    if (stck->next->op != INTEGER_)                                            \
    execerror("integer as second parameter", NAME)
#define CHARACTER(NAME)                                                        \
    if (stck->op != CHAR_)                                                     \
    execerror("character", NAME)
#define INTEGERS2(NAME)                                                        \
    if (stck->op != INTEGER_ || stck->next->op != INTEGER_)                    \
    execerror("two integers", NAME)
#define NUMERICTYPE(NAME)                                                      \
    if (stck->op != INTEGER_ && stck->op != CHAR_ && stck->op != BOOLEAN_)     \
    execerror("numeric", NAME)
#define NUMERIC2(NAME)                                                         \
    if (stck->next->op != INTEGER_ && stck->next->op != CHAR_)                 \
    execerror("numeric second parameter", NAME)
#else
#define ONEPARAM(NAME)
#define TWOPARAMS(NAME)
#define THREEPARAMS(NAME)
#define FOURPARAMS(NAME)
#define FIVEPARAMS(NAME)
#define ONEQUOTE(NAME)
#define TWOQUOTES(NAME)
#define THREEQUOTES(NAME)
#define FOURQUOTES(NAME)
#define SAME2TYPES(NAME)
#define STRING(NAME)
#define STRING2(NAME)
#define INTEGER(NAME)
#define INTEGER2(NAME)
#define CHARACTER(NAME)
#define INTEGERS2(NAME)
#define NUMERICTYPE(NAME)
#define NUMERIC2(NAME)
#endif
#define FLOATABLE (stck->op == INTEGER_ || stck->op == FLOAT_)
#define FLOATABLE2                                                             \
    ((stck->op == FLOAT_ && stck->next->op == FLOAT_)                          \
        || (stck->op == FLOAT_ && stck->next->op == INTEGER_)                  \
        || (stck->op == INTEGER_ && stck->next->op == FLOAT_))
#ifdef RUNTIME_CHECKS
#define FLOAT(NAME)                                                            \
    if (!FLOATABLE)                                                            \
        execerror("float or integer", NAME);
#define FLOAT2(NAME)                                                           \
    if (!(FLOATABLE2 || (stck->op == INTEGER_ && stck->next->op == INTEGER_))) \
    execerror("two floats or integers", NAME)
#else
#define FLOAT(NAME)
#define FLOAT2(NAME)
#endif
#define FLOATVAL (stck->op == FLOAT_ ? stck->u.dbl : (double)stck->u.num)
#define FLOATVAL2                                                              \
    (stck->next->op == FLOAT_ ? stck->next->u.dbl : (double)stck->next->u.num)
#define FLOAT_U(OPER)                                                          \
    if (FLOATABLE) {                                                           \
        UNARY(FLOAT_NEWNODE, OPER(FLOATVAL));                                  \
        return;                                                                \
    }
#define FLOAT_P(OPER)                                                          \
    if (FLOATABLE2) {                                                          \
        BINARY(FLOAT_NEWNODE, OPER(FLOATVAL2, FLOATVAL));                      \
        return;                                                                \
    }
#define FLOAT_I(OPER)                                                          \
    if (FLOATABLE2) {                                                          \
        BINARY(FLOAT_NEWNODE, (FLOATVAL2)OPER(FLOATVAL));                      \
        return;                                                                \
    }
#ifdef RUNTIME_CHECKS
#define FILE(NAME)                                                             \
    if (stck->op != FILE_ || stck->u.fil == NULL)                              \
    execerror("file", NAME)
#define CHECKZERO(NAME)                                                        \
    if (stck->u.num == 0)                                                      \
    execerror("non-zero operand", NAME)
#define LIST(NAME)                                                             \
    if (stck->op != LIST_)                                                     \
    execerror("list", NAME)
#define LIST2(NAME)                                                            \
    if (stck->next->op != LIST_)                                               \
    execerror("list as second parameter", NAME)
#define USERDEF(NAME)                                                          \
    if (stck->op != USR_)                                                      \
    execerror("user defined symbol", NAME)
#define CHECKLIST(OPR, NAME)                                                   \
    if (OPR != LIST_)                                                          \
    execerror("internal list", NAME)
#define CHECKSETMEMBER(NODE, NAME)                                             \
    if ((NODE->op != INTEGER_ && NODE->op != CHAR_) || NODE->u.num >= SETSIZE) \
    execerror("small numeric", NAME)
#define CHECKEMPTYSET(SET, NAME)                                               \
    if (SET == 0)                                                              \
    execerror("non-empty set", NAME)
#define CHECKEMPTYSTRING(STRING, NAME)                                         \
    if (*STRING == '\0')                                                       \
    execerror("non-empty string", NAME)
#define CHECKEMPTYLIST(LIST, NAME)                                             \
    if (LIST == NULL)                                                          \
    execerror("non-empty list", NAME)
#define INDEXTOOLARGE(NAME) execerror("smaller index", NAME)
#define BADAGGREGATE(NAME) execerror("aggregate parameter", NAME)
#define BADDATA(NAME) execerror("different type", NAME)
#else
#define FILE(NAME)
#define CHECKZERO(NAME)
#define LIST(NAME)
#define LIST2(NAME)
#define USERDEF(NAME)
#define CHECKLIST(OPR, NAME)
#define CHECKSETMEMBER(NODE, NAME)
#define CHECKEMPTYSET(SET, NAME)
#define CHECKEMPTYSTRING(STRING, NAME)
#define CHECKEMPTYLIST(LIST, NAME)
#define INDEXTOOLARGE(NAME)
#define BADAGGREGATE(NAME)
#define BADDATA(NAME)
#endif

#define DMP dump->u.lis
#define DMP1 dump1->u.lis
#define DMP2 dump2->u.lis
#define DMP3 dump3->u.lis
#define DMP4 dump4->u.lis
#define DMP5 dump5->u.lis
#define SAVESTACK dump = LIST_NEWNODE(stck, dump)
#define SAVED1 DMP
#define SAVED2 DMP->next
#define SAVED3 DMP->next->next
#define SAVED4 DMP->next->next->next
#define SAVED5 DMP->next->next->next->next
#define SAVED6 DMP->next->next->next->next->next

#define POP(X) X = X->next

#define NULLARY(CONSTRUCTOR, VALUE) stck = CONSTRUCTOR(VALUE, stck)
#define UNARY(CONSTRUCTOR, VALUE) stck = CONSTRUCTOR(VALUE, stck->next)
#define BINARY(CONSTRUCTOR, VALUE) stck = CONSTRUCTOR(VALUE, stck->next->next)
#define GNULLARY(TYPE, VALUE) stck = newnode(TYPE, (VALUE), stck)
#define GUNARY(TYPE, VALUE) stck = newnode(TYPE, (VALUE), stck->next)
#define GBINARY(TYPE, VALUE) stck = newnode(TYPE, (VALUE), stck->next->next)
#define GTERNARY(TYPE, VALUE)                                                  \
    stck = newnode(TYPE, (VALUE), stck->next->next->next)

#define GETSTRING(NODE)                                                        \
    (NODE->op == STRING_                                                       \
            ? NODE->u.str                                                      \
            : (NODE->op == USR_ ? NODE->u.ent->name : opername(NODE->op)))

/* - - - -  O P E R A N D S   - - - - */

#define PUSH(PROCEDURE, CONSTRUCTOR, VALUE)                                    \
    PRIVATE void PROCEDURE(void) { NULLARY(CONSTRUCTOR, VALUE); }
#if 0
PUSH(true_, BOOLEAN_NEWNODE, 1L)			/* constants	*/
PUSH(false_, BOOLEAN_NEWNODE, 0L)
PUSH(maxint_, INTEGER_NEWNODE, (long_t)MAXINT)
#endif
PUSH(setsize_, INTEGER_NEWNODE, (long_t)SETSIZE)
PUSH(symtabmax_, INTEGER_NEWNODE, (long_t)SYMTABMAX)
PUSH(memorymax_, INTEGER_NEWNODE, (long_t)MEMORYMAX)
PUSH(stdin_, FILE_NEWNODE, stdin)
PUSH(stdout_, FILE_NEWNODE, stdout)
PUSH(stderr_, FILE_NEWNODE, stderr)
PUSH(dump_, LIST_NEWNODE, dump) /* variables	*/
PUSH(conts_, LIST_NEWNODE, LIST_NEWNODE(conts->u.lis->next, conts->next))
PUSH(symtabindex_, INTEGER_NEWNODE, (long_t)LOC2INT(symtabindex))
PUSH(rand_, INTEGER_NEWNODE, (long_t)my_rand())
/* this is now in utils.c
PUSH(memoryindex_, INTEGER_NEWNODE, MEM2INT(memoryindex))
*/
PUSH(echo_, INTEGER_NEWNODE, (long_t)echoflag)
PUSH(autoput_, INTEGER_NEWNODE, (long_t)autoput)
PUSH(undeferror_, INTEGER_NEWNODE, (long_t)undeferror)
PUSH(clock_, INTEGER_NEWNODE, (long_t)(clock() - startclock))
PUSH(time_, INTEGER_NEWNODE, (long_t)time(NULL))
PUSH(argc_, INTEGER_NEWNODE, (long_t)g_argc)

PUBLIC void stack_(void) { NULLARY(LIST_NEWNODE, stck); }

/* - - - - -   O P E R A T O R S   - - - - - */

PRIVATE void id_(void) { /* do nothing */ }

PRIVATE void unstack_(void)
{
    ONEPARAM("unstack");
    LIST("unstack");
    stck = stck->u.lis;
}

/*
PRIVATE void newstack_(void)
{
    stck = NULL;
}
*/

/* - - -   STACK   - - - */

PRIVATE void name_(void)
{
    ONEPARAM("name");
    UNARY(STRING_NEWNODE,
        stck->op == USR_ ? stck->u.ent->name : opername(stck->op));
}

PRIVATE void intern_(void)
{
#if defined(RUNTIME_CHECKS)
    char* p;
#endif
    ONEPARAM("intern");
    STRING("intern");
    strncpy(ident, stck->u.str, ALEN);
    ident[ALEN - 1] = 0;
#if defined(RUNTIME_CHECKS) && defined(CORRECT_INTERN_LOOKUP)
    p = 0;
    if (ident[0] == '-' || !strchr("(#)[]{}.;'\"0123456789", ident[0]))
        for (p = ident + 1; *p; p++)
            if (!isalnum((int)*p) && !strchr("=_-", *p))
                break;
    if (!p || *p)
        execerror("valid name", ident);
#endif
    HashValue(ident);
    lookup(0);
    if (location < firstlibra) {
        bucket.proc = location->u.proc;
        GUNARY(LOC2INT(location), bucket);
    } else
        UNARY(USR_NEWNODE, location);
}

PRIVATE void getenv_(void)
{
    char* str;

    ONEPARAM("getenv");
    STRING("getenv");
    if ((str = getenv(stck->u.str)) == 0)
        str = "";
    UNARY(STRING_NEWNODE, str);
}

PRIVATE void body_(void)
{
    ONEPARAM("body");
    USERDEF("body");
    UNARY(LIST_NEWNODE, stck->u.ent->u.body);
}

PRIVATE void pop_(void)
{
    ONEPARAM("pop");
    POP(stck);
}

PRIVATE void swap_(void)
{
    TWOPARAMS("swap");
    SAVESTACK;
    GBINARY(SAVED1->op, SAVED1->u);
    GNULLARY(SAVED2->op, SAVED2->u);
    POP(dump);
}

PRIVATE void rollup_(void)
{
    THREEPARAMS("rollup");
    SAVESTACK;
    GTERNARY(SAVED1->op, SAVED1->u);
    GNULLARY(SAVED3->op, SAVED3->u);
    GNULLARY(SAVED2->op, SAVED2->u);
    POP(dump);
}

PRIVATE void rolldown_(void)
{
    THREEPARAMS("rolldown");
    SAVESTACK;
    GTERNARY(SAVED2->op, SAVED2->u);
    GNULLARY(SAVED1->op, SAVED1->u);
    GNULLARY(SAVED3->op, SAVED3->u);
    POP(dump);
}

PRIVATE void rotate_(void)
{
    THREEPARAMS("rotate");
    SAVESTACK;
    GTERNARY(SAVED1->op, SAVED1->u);
    GNULLARY(SAVED2->op, SAVED2->u);
    GNULLARY(SAVED3->op, SAVED3->u);
    POP(dump);
}

PRIVATE void dup_(void)
{
    ONEPARAM("dup");
    GNULLARY(stck->op, stck->u);
}

#define DIPPED(PROCEDURE, NAME, PARAMCOUNT, ARGUMENT)                          \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        PARAMCOUNT(NAME);                                                      \
        SAVESTACK;                                                             \
        POP(stck);                                                             \
        ARGUMENT();                                                            \
        GNULLARY(SAVED1->op, SAVED1->u);                                       \
        POP(dump);                                                             \
    }

DIPPED(popd_, "popd", TWOPARAMS, pop_)
DIPPED(dupd_, "dupd", TWOPARAMS, dup_)
DIPPED(swapd_, "swapd", THREEPARAMS, swap_)
DIPPED(rolldownd_, "rolldownd", FOURPARAMS, rolldown_)
DIPPED(rollupd_, "rollupd", FOURPARAMS, rollup_)
DIPPED(rotated_, "rotated", FOURPARAMS, rotate_)

/* - - -   BOOLEAN   - - - */

#define ANDORXOR(PROCEDURE, NAME, OPER1, OPER2)                                \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        SAME2TYPES(NAME);                                                      \
        switch (stck->next->op) {                                              \
        case SET_:                                                             \
            BINARY(                                                            \
                SET_NEWNODE, (long_t)(stck->next->u.set OPER1 stck->u.set));   \
            return;                                                            \
        case BOOLEAN_:                                                         \
        case CHAR_:                                                            \
        case INTEGER_:                                                         \
        case LIST_:                                                            \
            BINARY(BOOLEAN_NEWNODE,                                            \
                (long_t)(stck->next->u.num OPER2 stck->u.num));                \
            return;                                                            \
        default:                                                               \
            BADDATA(NAME);                                                     \
        }                                                                      \
    }
ANDORXOR(and_, "and", &, &&)
ANDORXOR(or_, "or", |, ||)
ANDORXOR(xor_, "xor", ^, !=)

/* - - -   INTEGER   - - - */

#define ORDCHR(PROCEDURE, NAME, RESULTTYP)                                     \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        NUMERICTYPE(NAME);                                                     \
        UNARY(RESULTTYP, stck->u.num);                                         \
    }
ORDCHR(ord_, "ord", INTEGER_NEWNODE)
ORDCHR(chr_, "chr", CHAR_NEWNODE)

PRIVATE void abs_(void)
{
    ONEPARAM("abs");
    /* start new */
    FLOAT("abs");
    if (stck->op == INTEGER_) {
        if (stck->u.num < 0)
            UNARY(INTEGER_NEWNODE, -stck->u.num);
        return;
    }
    /* end new */
    FLOAT_U(fabs);
#if 0
    INTEGER("abs");
    if (stck->u.num < 0) UNARY(INTEGER_NEWNODE, - stck->u.num);
#endif
}

PRIVATE double fsgn(double f)
{
    if (f < 0)
        return -1.0;
    if (f > 0)
        return 1.0;
    return 0.0;
}

PRIVATE void sign_(void)
{
    ONEPARAM("sign");
    /* start new */
    FLOAT("sign");
    if (stck->op == INTEGER_) {
        if (stck->u.num != 0 && stck->u.num != 1)
            UNARY(INTEGER_NEWNODE, stck->u.num > 0 ? 1 : -1);
        return;
    }
    /* end new */
    FLOAT_U(fsgn);
#if 0
    INTEGER("sign");
    if (stck->u.num < 0) UNARY(INTEGER_NEWNODE, -1L);
    else if (stck->u.num > 0) UNARY(INTEGER_NEWNODE, 1L);
#endif
}

PRIVATE void neg_(void)
{
    ONEPARAM("neg");
    /* start new */
    FLOAT("neg");
    if (stck->op == INTEGER_) {
        if (stck->u.num)
            UNARY(INTEGER_NEWNODE, -stck->u.num);
        return;
    }
    /* end new */
    FLOAT_U(-);
#if 0
    INTEGER("neg");
    UNARY(INTEGER_NEWNODE, -stck->u.num);
#endif
}

/* probably no longer needed:
#define MULDIV(PROCEDURE, NAME, OPER, CHECK)			\
PRIVATE void PROCEDURE(void)					\
{   TWOPARAMS(NAME);						\
    FLOAT_I(OPER);						\
    INTEGERS2(NAME);						\
    CHECK;							\
    BINARY(INTEGER_NEWNODE, stck->next->u.num OPER stck->u.num); }
MULDIV(mul_, "*", *, )
MULDIV(divide_, "/", /, CHECKZERO("/"))
*/

PRIVATE void mul_(void)
{
    TWOPARAMS("*");
    FLOAT_I(*);
    INTEGERS2("*");
    BINARY(INTEGER_NEWNODE, stck->next->u.num * stck->u.num);
}

PRIVATE void divide_(void)
{
#ifdef RUNTIME_CHECKS
    TWOPARAMS("/");
    if ((stck->op == FLOAT_ && stck->u.dbl == 0.0)
        || (stck->op == INTEGER_ && stck->u.num == 0))
        execerror("non-zero divisor", "/");
#endif
    FLOAT_I(/);
    INTEGERS2("/");
    BINARY(INTEGER_NEWNODE, stck->next->u.num / stck->u.num);
}

PRIVATE void rem_(void)
{
    TWOPARAMS("rem");
    FLOAT_P(fmod);
    INTEGERS2("rem");
    CHECKZERO("rem");
    BINARY(INTEGER_NEWNODE, stck->next->u.num % stck->u.num);
}

PRIVATE void div_(void)
{
#ifdef BIT_32
    ldiv_t result;
#else
    lldiv_t result;
#endif
    TWOPARAMS("div");
    INTEGERS2("div");
    CHECKZERO("div");
#ifdef BIT_32
    result = ldiv(stck->next->u.num, stck->u.num);
#else
    result = lldiv(stck->next->u.num, stck->u.num);
#endif
    BINARY(INTEGER_NEWNODE, result.quot);
    NULLARY(INTEGER_NEWNODE, result.rem);
}

PRIVATE void strtol_(void)
{
    TWOPARAMS("strtol");
    SAVESTACK;
    INTEGER("strtol");
    POP(stck);
    STRING("strtol");
#ifdef BIT_32
    UNARY(INTEGER_NEWNODE, strtol(SAVED2->u.str, NULL, SAVED1->u.num));
#else
    UNARY(INTEGER_NEWNODE, strtoll(SAVED2->u.str, NULL, SAVED1->u.num));
#endif
    POP(dump);
}

PRIVATE void strtod_(void)
{
    ONEPARAM("strtod");
    STRING("strtod");
    UNARY(FLOAT_NEWNODE, strtod(stck->u.str, NULL));
}

PRIVATE void format_(void)
{
    int width, prec;
    char spec, format[7], *result;
#ifdef USE_SNPRINTF
    int leng;
#endif
    FOURPARAMS("format");
    INTEGER("format");
    INTEGER2("format");
    prec = stck->u.num;
    POP(stck);
    width = stck->u.num;
    POP(stck);
    CHARACTER("format");
    spec = (char)stck->u.num;
    POP(stck);
#ifdef RUNTIME_CHECKS
    if (!strchr("dioxX", spec))
        execerror("one of: d i o x X", "format");
#endif
    strcpy(format, "%*.*ld");
    format[5] = spec;
#ifdef USE_SNPRINTF
    leng = snprintf(0, 0, format, width, prec, stck->u.num) + 1;
    result = malloc(leng + 1);
#else
    result = malloc(INPLINEMAX); /* should be sufficient */
#endif
    NUMERICTYPE("format");
#ifdef USE_SNPRINTF
    snprintf(result, leng, format, width, prec, stck->u.num);
#else
    sprintf(result, format, width, prec, stck->u.num);
#endif
    UNARY(STRING_NEWNODE, result);
}

PRIVATE void formatf_(void)
{
    int width, prec;
    char spec, format[7], *result;
#ifdef USE_SNPRINTF
    int leng;
#endif
    FOURPARAMS("formatf");
    INTEGER("formatf");
    INTEGER2("formatf");
    prec = stck->u.num;
    POP(stck);
    width = stck->u.num;
    POP(stck);
    CHARACTER("formatf");
    spec = (char)stck->u.num;
    POP(stck);
#ifdef RUNTIME_CHECKS
    if (!strchr("eEfgG", spec))
        execerror("one of: e E f g G", "formatf");
#endif
    strcpy(format, "%*.*lg");
    format[5] = spec;
#ifdef USE_SNPRINTF
    leng = snprintf(0, 0, format, width, prec, stck->u.num) + 1;
    result = malloc(leng + 1);
#else
    result = malloc(INPLINEMAX); /* should be sufficient */
#endif
    FLOAT("formatf");
#ifdef USE_SNPRINTF
    snprintf(result, leng, format, width, prec, stck->u.dbl);
#else
    sprintf(result, format, width, prec, stck->u.dbl);
#endif
    UNARY(STRING_NEWNODE, result);
}

/* - - -   TIME   - - - */

#ifndef USE_TIME_REC
void localtime_r(time_t* t, struct tm* tm) { *tm = *localtime(t); }

void gmtime_r(time_t* t, struct tm* tm) { *tm = *gmtime(t); }
#endif

#define UNMKTIME(PROCEDURE, NAME, FUNC)                                        \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        struct tm t;                                                           \
        long_t wday;                                                           \
        time_t timval;                                                         \
        ONEPARAM(NAME);                                                        \
        INTEGER(NAME);                                                         \
        timval = stck->u.num;                                                  \
        FUNC(&timval, &t);                                                     \
        wday = t.tm_wday;                                                      \
        if (wday == 0)                                                         \
            wday = 7;                                                          \
        dump1 = LIST_NEWNODE(NULL, dump1);                                     \
        DMP1 = INTEGER_NEWNODE(wday, DMP1);                                    \
        DMP1 = INTEGER_NEWNODE((long_t)t.tm_yday, DMP1);                       \
        DMP1 = BOOLEAN_NEWNODE((long_t)t.tm_isdst, DMP1);                      \
        DMP1 = INTEGER_NEWNODE((long_t)t.tm_sec, DMP1);                        \
        DMP1 = INTEGER_NEWNODE((long_t)t.tm_min, DMP1);                        \
        DMP1 = INTEGER_NEWNODE((long_t)t.tm_hour, DMP1);                       \
        DMP1 = INTEGER_NEWNODE((long_t)t.tm_mday, DMP1);                       \
        DMP1 = INTEGER_NEWNODE((long_t)(t.tm_mon + 1), DMP1);                  \
        DMP1 = INTEGER_NEWNODE((long_t)(t.tm_year + 1900), DMP1);              \
        UNARY(LIST_NEWNODE, DMP1);                                             \
        POP(dump1);                                                            \
    }
UNMKTIME(localtime_, "localtime", localtime_r)
UNMKTIME(gmtime_, "gmtime", gmtime_r)

PRIVATE void decode_time(struct tm* t)
{
    Node* p;
    t->tm_year = t->tm_mon = t->tm_mday = t->tm_hour = t->tm_min = t->tm_sec
        = t->tm_isdst = t->tm_yday = t->tm_wday = 0;
    p = stck->u.lis;
    if (p && p->op == INTEGER_) {
        t->tm_year = p->u.num - 1900;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_mon = p->u.num - 1;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_mday = p->u.num;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_hour = p->u.num;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_min = p->u.num;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_sec = p->u.num;
        POP(p);
    }
    if (p && p->op == BOOLEAN_) {
        t->tm_isdst = p->u.num;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_yday = p->u.num;
        POP(p);
    }
    if (p && p->op == INTEGER_) {
        t->tm_wday = p->u.num;
        POP(p);
    }
}

PRIVATE void mktime_(void)
{
    struct tm t;

    ONEPARAM("mktime");
    LIST("mktime");
    decode_time(&t);
    UNARY(INTEGER_NEWNODE, (long_t)mktime(&t));
}

PRIVATE void strftime_(void)
{
    struct tm t;
    char *fmt, *result;
    size_t length;

    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = stck->u.str;
    POP(stck);
    LIST("strftime");
    decode_time(&t);
    length = INPLINEMAX;
    result = malloc(length);
    strftime(result, length, fmt, &t);
    UNARY(STRING_NEWNODE, result);
}

/* - - -   FLOAT   - - - */

#define UFLOAT(PROCEDURE, NAME, FUNC)                                          \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        FLOAT(NAME);                                                           \
        UNARY(FLOAT_NEWNODE, FUNC(FLOATVAL));                                  \
    }
UFLOAT(acos_, "acos", acos)
UFLOAT(asin_, "asin", asin)
UFLOAT(atan_, "atan", atan)
UFLOAT(ceil_, "ceil", ceil)
UFLOAT(cos_, "cos", cos)
UFLOAT(cosh_, "cosh", cosh)
UFLOAT(exp_, "exp", exp)
UFLOAT(floor_, "floor", floor)
UFLOAT(log_, "log", log)
UFLOAT(log10_, "log10", log10)
UFLOAT(sin_, "sin", sin)
UFLOAT(sinh_, "sinh", sinh)
UFLOAT(sqrt_, "sqrt", sqrt)
UFLOAT(tan_, "tan", tan)
UFLOAT(tanh_, "tanh", tanh)

#define BFLOAT(PROCEDURE, NAME, FUNC)                                          \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        FLOAT2(NAME);                                                          \
        BINARY(FLOAT_NEWNODE, FUNC(FLOATVAL2, FLOATVAL));                      \
    }
BFLOAT(atan2_, "atan2", atan2)
BFLOAT(pow_, "pow", pow)

PRIVATE void frexp_(void)
{
    int exp;

    ONEPARAM("frexp");
    FLOAT("frexp");
    UNARY(FLOAT_NEWNODE, frexp(FLOATVAL, &exp));
    NULLARY(INTEGER_NEWNODE, (long_t)exp);
}

PRIVATE void modf_(void)
{
    double exp;

    ONEPARAM("modf");
    FLOAT("modf");
    UNARY(FLOAT_NEWNODE, modf(FLOATVAL, &exp));
    NULLARY(FLOAT_NEWNODE, exp);
}

PRIVATE void ldexp_(void)
{
    int exp;

    TWOPARAMS("ldexp");
    INTEGER("ldexp");
    exp = stck->u.num;
    POP(stck);
    FLOAT("ldexp");
    UNARY(FLOAT_NEWNODE, ldexp(FLOATVAL, exp));
}

PRIVATE void trunc_(void)
{
    ONEPARAM("trunc");
    FLOAT("trunc");
    UNARY(INTEGER_NEWNODE, (long_t)FLOATVAL);
}

/* - - -   NUMERIC   - - - */

#define PREDSUCC(PROCEDURE, NAME, OPER)                                        \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        NUMERICTYPE(NAME);                                                     \
        if (stck->op == CHAR_)                                                 \
            UNARY(CHAR_NEWNODE, stck->u.num OPER 1);                           \
        else                                                                   \
            UNARY(INTEGER_NEWNODE, stck->u.num OPER 1);                        \
    }
PREDSUCC(pred_, "pred", -)
PREDSUCC(succ_, "succ", +)

#define PLUSMINUS(PROCEDURE, NAME, OPER)                                       \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        FLOAT_I(OPER);                                                         \
        INTEGER(NAME);                                                         \
        NUMERIC2(NAME);                                                        \
        if (stck->next->op == CHAR_)                                           \
            BINARY(CHAR_NEWNODE, stck->next->u.num OPER stck->u.num);          \
        else                                                                   \
            BINARY(INTEGER_NEWNODE, stck->next->u.num OPER stck->u.num);       \
    }
PLUSMINUS(plus_, "+", +)
PLUSMINUS(minus_, "-", -)

#define MAXMIN(PROCEDURE, NAME, OPER)                                          \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        if (FLOATABLE2) {                                                      \
            BINARY(FLOAT_NEWNODE,                                              \
                FLOATVAL OPER FLOATVAL2 ? FLOATVAL2 : FLOATVAL);               \
            return;                                                            \
        }                                                                      \
        SAME2TYPES(NAME);                                                      \
        NUMERICTYPE(NAME);                                                     \
        if (stck->op == CHAR_)                                                 \
            BINARY(CHAR_NEWNODE, stck->u.num OPER stck->next->u.num            \
                    ? stck->next->u.num                                        \
                    : stck->u.num);                                            \
        else                                                                   \
            BINARY(INTEGER_NEWNODE, stck->u.num OPER stck->next->u.num         \
                    ? stck->next->u.num                                        \
                    : stck->u.num);                                            \
    }
MAXMIN(max_, "max", <)
MAXMIN(min_, "min", >)

#if defined(CORRECT_INHAS_COMPARE) || defined(CORRECT_TYPE_COMPARE)            \
    || defined(CORRECT_CASE_COMPARE)
PRIVATE double Compare(Node* first, Node* second, int* error)
{
    *error = 0;
    switch (first->op) {
    case USR_:
        switch (second->op) {
        case USR_:
            return strcmp(first->u.ent->name, second->u.ent->name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(first->u.ent->name, second->u.str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(first->u.ent->name, opername(second->op));
        }
        break;
    case ANON_FUNCT_:
        switch (second->op) {
        case USR_:
            break;
        case ANON_FUNCT_:
            return (size_t)first->u.proc - (size_t)second->u.proc;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case BOOLEAN_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return first->u.num - second->u.num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return first->u.num - second->u.dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case CHAR_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return first->u.num - second->u.num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return first->u.num - second->u.dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case INTEGER_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return first->u.num - second->u.num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return first->u.num - second->u.dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case SET_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            break;
        case SET_:
            return first->u.set - second->u.set;
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case STRING_:
        switch (second->op) {
        case USR_:
            return strcmp(first->u.str, second->u.ent->name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(first->u.str, second->u.str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(first->u.str, opername(second->op));
        }
        break;
    case LIST_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case FLOAT_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return first->u.dbl - second->u.num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return first->u.dbl - second->u.dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case FILE_:
        switch (second->op) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
            break;
        case FILE_:
            return first->u.fil - second->u.fil;
        default:
            break;
        }
        break;
    default:
        switch (second->op) {
        case USR_:
            return strcmp(opername(first->op), second->u.ent->name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(opername(first->op), second->u.str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(opername(first->op), opername(second->op));
        }
        break;
    }
    *error = 1;
    return 0;
}
#endif

#ifdef CORRECT_TYPE_COMPARE
#define COMPREL(PROCEDURE, NAME, CONSTRUCTOR, OPR, SETCMP)                     \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        double cmp;                                                            \
        int comp = 0, error, i, j;                                             \
        TWOPARAMS(NAME);                                                       \
        if (stck->op == SET_) {                                                \
            i = stck->next->u.num;                                             \
            j = stck->u.num;                                                   \
            comp = SETCMP;                                                     \
        } else {                                                               \
            cmp = Compare(stck->next, stck, &error);                           \
            if (error)                                                         \
                BADDATA(NAME);                                                 \
            else {                                                             \
                comp = (int)(cmp OPR 0);                                       \
                if (comp < 0)                                                  \
                    comp = -1;                                                 \
                else if (comp > 0)                                             \
                    comp = 1;                                                  \
            }                                                                  \
        }                                                                      \
        stck = CONSTRUCTOR(comp, stck->next->next);                            \
    }
#else
#define COMPREL(PROCEDURE, NAME, CONSTRUCTOR, OPR)                             \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        int comp = 0;                                                          \
        TWOPARAMS(NAME);                                                       \
        switch (stck->op) {                                                    \
        case BOOLEAN_:                                                         \
        case CHAR_:                                                            \
        case INTEGER_:                                                         \
            if (FLOATABLE2)                                                    \
                comp = FLOATVAL2 - FLOATVAL OPR 0;                             \
            else                                                               \
                comp = stck->next->u.num - stck->u.num OPR 0;                  \
            break;                                                             \
        case FLOAT_:                                                           \
            if (FLOATABLE2)                                                    \
                comp = FLOATVAL2 - FLOATVAL OPR 0;                             \
            else                                                               \
                comp = 0;                                                      \
            break;                                                             \
        case SET_: {                                                           \
            int i = 0;                                                         \
            while (i < SETSIZE && ((stck->next->u.set & ((long_t)1 << i))      \
                                      == (stck->u.set & ((long_t)1 << i))))    \
                ++i;                                                           \
            if (i == SETSIZE)                                                  \
                i = 0;                                                         \
            else                                                               \
                ++i;                                                           \
            if (!(stck->u.set & ((long_t)1 << i)))                             \
                i = -i;                                                        \
            comp = i OPR 0;                                                    \
            break;                                                             \
        }                                                                      \
        case LIST_:                                                            \
            BADDATA(NAME);                                                     \
        default:                                                               \
            if (stck->next->op == LIST_)                                       \
                BADDATA(NAME);                                                 \
            comp = strcmp(GETSTRING(stck->next), GETSTRING(stck)) OPR 0;       \
            break;                                                             \
        }                                                                      \
        stck = CONSTRUCTOR(comp, stck->next->next);                            \
    }

#endif

#ifdef CORRECT_TYPE_COMPARE
COMPREL(eql_, "=", BOOLEAN_NEWNODE, ==, i == j)
COMPREL(neql_, "!=", BOOLEAN_NEWNODE, !=, i != j)
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))
COMPREL(leql_, "<=", BOOLEAN_NEWNODE, <=, !(i & ~j))
COMPREL(greater_, ">", BOOLEAN_NEWNODE, >, i != j && !(j & ~i))
COMPREL(geql_, ">=", BOOLEAN_NEWNODE, >=, !(j & ~i))
COMPREL(compare_, "compare", INTEGER_NEWNODE, +, i - j < 0 ? -1 : i - j > 0)
#else
COMPREL(eql_, "=", BOOLEAN_NEWNODE, ==)
COMPREL(neql_, "!=", BOOLEAN_NEWNODE, !=)
COMPREL(less_, "<", BOOLEAN_NEWNODE, <)
COMPREL(leql_, "<=", BOOLEAN_NEWNODE, <=)
COMPREL(greater_, ">", BOOLEAN_NEWNODE, >)
COMPREL(geql_, ">=", BOOLEAN_NEWNODE, >=)
COMPREL(compare_, "compare", INTEGER_NEWNODE, +)
#endif

#ifdef SAMETYPE_BUILTIN
PRIVATE void sametype_(void)
{
    TWOPARAMS("sametype");
    BINARY(BOOLEAN_NEWNODE, stck->op == stck->next->op);
}
#endif

/* - - -   FILES AND STREAMS   - - - */

PRIVATE void fopen_(void)
{
    TWOPARAMS("fopen");
    STRING("fopen");
    STRING2("fopen");
    BINARY(FILE_NEWNODE, fopen(stck->next->u.str, stck->u.str));
}

PRIVATE void fclose_(void)
{
    ONEPARAM("fclose");
    if (stck->op == FILE_ && stck->u.fil == NULL) {
        POP(stck);
        return;
    }
    FILE("fclose");
    fclose(stck->u.fil);
    POP(stck);
}

PRIVATE void fflush_(void)
{
    ONEPARAM("fflush");
    FILE("fflush");
    fflush(stck->u.fil);
}

PRIVATE void fremove_(void)
{
    ONEPARAM("fremove");
    STRING("fremove");
    UNARY(BOOLEAN_NEWNODE, (long_t)!remove(stck->u.str));
}

PRIVATE void frename_(void)
{
    TWOPARAMS("frename");
    STRING("frename");
    STRING2("frename");
    BINARY(BOOLEAN_NEWNODE, (long_t)!rename(stck->next->u.str, stck->u.str));
}

#define FILEGET(PROCEDURE, NAME, CONSTRUCTOR, EXPR)                            \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        FILE(NAME);                                                            \
        NULLARY(CONSTRUCTOR, EXPR);                                            \
    }
FILEGET(feof_, "feof", BOOLEAN_NEWNODE, (long_t)feof(stck->u.fil))
FILEGET(ferror_, "ferror", BOOLEAN_NEWNODE, (long_t)ferror(stck->u.fil))
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, (long_t)getc(stck->u.fil))
FILEGET(ftell_, "ftell", INTEGER_NEWNODE, ftell(stck->u.fil))

#ifdef GETCH_AS_BUILTIN
PRIVATE void getch_(void) { NULLARY(CHAR_NEWNODE, (long_t)getchar()); }
#endif

PRIVATE void fgets_(void)
{
    int length = 0, size = INPLINEMAX;
    char *buff = NULL, *newbuff;

    ONEPARAM("fgets");
    FILE("fgets");
    for (;;) {
        if ((newbuff = realloc(buff, size)) == 0)
            break;
        buff = newbuff;
        if (fgets(buff + length, size - length, stck->u.fil) == NULL) {
            buff[length] = 0;
            break;
        }
        if (strchr(buff, '\n'))
            break;
        length = strlen(buff);
        size = size * 2;
    }
    NULLARY(STRING_NEWNODE, buff);
}

PRIVATE void fput_(void)
{
    FILE* stm;

#ifdef RUNTIME_CHECKS
    TWOPARAMS("fput");
    stm = NULL;
    if (stck->next->op != FILE_ || (stm = stck->next->u.fil) == NULL)
        execerror("file", "fput");
#else
    stm = stck->next->u.fil;
#endif
    writefactor(stck, stm);
    fprintf(stm, " ");
    POP(stck);
}

#ifdef FGET_FROM_FILE
PRIVATE void fget_(void)
{
    FILE* stm = NULL;

#ifdef RUNTIME_CHECKS
    ONEPARAM("fget");
    if (stck->op != FILE_ || (stm = stck->u.fil) == NULL)
        execerror("file", "fget");
#else
    stm = stck->u.fil;
#endif
    redirect(stm);
    getsym();
    readfactor(0);
}
#endif

PRIVATE void fputch_(void)
{
    int ch;

    TWOPARAMS("fputch");
    INTEGER("fputch");
    ch = stck->u.num;
    POP(stck);
    FILE("fputch");
    putc(ch, stck->u.fil);
}

PRIVATE void fputchars_(void) /* suggested by Heiko Kuhrt, as "fputstring_" */
{
    FILE* stm;

#ifdef RUNTIME_CHECKS
    TWOPARAMS("fputchars");
    stm = NULL;
    if (stck->next->op != FILE_ || (stm = stck->next->u.fil) == NULL)
        execerror("file", "fputchars");
#else
    stm = stck->next->u.fil;
#endif
    fprintf(stm, "%s", stck->u.str);
    POP(stck);
}

PRIVATE void fread_(void)
{
    unsigned char* buf;
    long_t count;

    TWOPARAMS("fread");
    INTEGER("fread");
    count = stck->u.num;
    POP(stck);
    FILE("fread");
    buf = malloc(count);
    dump1 = LIST_NEWNODE(NULL, dump1);
    for (count = fread(buf, (size_t)1, (size_t)count, stck->u.fil) - 1;
         count >= 0; count--)
        DMP1 = INTEGER_NEWNODE((long_t)buf[count], DMP1);
    free(buf);
#ifdef CORRECT_FREAD_PARAM
    NULLARY(LIST_NEWNODE, DMP1);
#else
    UNARY(LIST_NEWNODE, DMP1);
#endif
    POP(dump1);
}

PRIVATE void fwrite_(void)
{
    int length, i;
    unsigned char* buff;
    Node* n;

    TWOPARAMS("fwrite");
    LIST("fwrite");
    for (n = stck->u.lis, length = 0; n; n = n->next, length++)
#ifdef RUNTIME_CHECKS
        if (n->op != INTEGER_)
            execerror("numeric list", "fwrite");
#else
        ;
#endif
    buff = malloc(length);
    for (n = stck->u.lis, i = 0; n; n = n->next, i++)
        buff[i] = (char)n->u.num;
    POP(stck);
    FILE("fwrite");
    fwrite(buff, (size_t)length, (size_t)1, stck->u.fil);
    free(buff);
}

PRIVATE void fseek_(void)
{
    long pos;
    int whence;

    THREEPARAMS("fseek");
    INTEGER("fseek");
    INTEGER2("fseek");
    whence = stck->u.num;
    POP(stck);
    pos = stck->u.num;
    POP(stck);
    FILE("fseek");
    NULLARY(BOOLEAN_NEWNODE, (long_t) !!fseek(stck->u.fil, pos, whence));
}

/* - - -   AGGREGATES   - - - */

PRIVATE void first_(void)
{
    ONEPARAM("first");
    switch (stck->op) {
    case LIST_:
        CHECKEMPTYLIST(stck->u.lis, "first");
        GUNARY(stck->u.lis->op, stck->u.lis->u);
        return;
    case STRING_:
        CHECKEMPTYSTRING(stck->u.str, "first");
        UNARY(CHAR_NEWNODE, (long_t) * (stck->u.str));
        return;
    case SET_: {
        int i = 0;
        CHECKEMPTYSET(stck->u.set, "first");
        while (!(stck->u.set & ((long_t)1 << i)))
            i++;
        UNARY(INTEGER_NEWNODE, i);
        return;
    }
    default:
        BADAGGREGATE("first");
    }
}

PRIVATE void rest_(void)
{
    ONEPARAM("rest");
    switch (stck->op) {
    case SET_: {
        int i = 0;
        CHECKEMPTYSET(stck->u.set, "rest");
        while (!(stck->u.set & ((long_t)1 << i)))
            i++;
        UNARY(SET_NEWNODE, stck->u.set & ~((long_t)1 << i));
        break;
    }
    case STRING_: {
        char* s = stck->u.str;
        CHECKEMPTYSTRING(s, "rest");
        UNARY(STRING_NEWNODE, ++s);
        break;
    }
    case LIST_:
        CHECKEMPTYLIST(stck->u.lis, "rest");
        UNARY(LIST_NEWNODE, stck->u.lis->next);
        return;
    default:
        BADAGGREGATE("rest");
    }
}

PRIVATE void uncons_(void)
{
    ONEPARAM("uncons");
    switch (stck->op) {
    case SET_: {
        int i = 0;
        long_t set = stck->u.set;
        CHECKEMPTYSET(set, "uncons");
        while (!(set & ((long_t)1 << i)))
            i++;
        UNARY(INTEGER_NEWNODE, i);
        NULLARY(SET_NEWNODE, set & ~((long_t)1 << i));
        break;
    }
    case STRING_: {
        char* s = stck->u.str;
        CHECKEMPTYSTRING(s, "uncons");
        UNARY(CHAR_NEWNODE, (long_t)*s);
        NULLARY(STRING_NEWNODE, ++s);
        break;
    }
    case LIST_:
        SAVESTACK;
        CHECKEMPTYLIST(SAVED1->u.lis, "uncons");
        GUNARY(SAVED1->u.lis->op, SAVED1->u.lis->u);
        NULLARY(LIST_NEWNODE, SAVED1->u.lis->next);
        POP(dump);
        return;
    default:
        BADAGGREGATE("uncons");
    }
}

PRIVATE void unswons_(void)
{
    ONEPARAM("unswons");
    switch (stck->op) {
    case SET_: {
        int i = 0;
        long_t set = stck->u.set;
        CHECKEMPTYSET(set, "unswons");
        while (!(set & ((long_t)1 << i)))
            i++;
        UNARY(SET_NEWNODE, set & ~((long_t)1 << i));
        NULLARY(INTEGER_NEWNODE, i);
        break;
    }
    case STRING_: {
        char* s = stck->u.str;
        CHECKEMPTYSTRING(s, "unswons");
        UNARY(STRING_NEWNODE, ++s);
        NULLARY(CHAR_NEWNODE, (long_t) * (--s));
        break;
    }
    case LIST_:
        SAVESTACK;
        CHECKEMPTYLIST(SAVED1->u.lis, "unswons");
        UNARY(LIST_NEWNODE, SAVED1->u.lis->next);
        GNULLARY(SAVED1->u.lis->op, SAVED1->u.lis->u);
        POP(dump);
        return;
    default:
        BADAGGREGATE("unswons");
    }
}

PRIVATE int equal_aux(Node* n1, Node* n2); /* forward */

PRIVATE int equal_list_aux(Node* n1, Node* n2)
{
    if (n1 == NULL && n2 == NULL)
        return 1;
    if (n1 == NULL || n2 == NULL)
        return 0;
    if (equal_aux(n1, n2))
        return equal_list_aux(n1->next, n2->next);
    else
        return 0;
}

PRIVATE int equal_aux(Node* n1, Node* n2)
{
#ifdef CORRECT_TYPE_COMPARE
    int error;
#endif
    if (n1 == NULL && n2 == NULL)
        return 1;
    if (n1 == NULL || n2 == NULL)
        return 0;
#ifdef CORRECT_TYPE_COMPARE
    if (n1->op == LIST_ && n2->op == LIST_)
        return equal_list_aux(n1->u.lis, n2->u.lis);
    return !Compare(n1, n2, &error) && !error;
#else
    switch (n1->op) {
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        if (n2->op != BOOLEAN_ && n2->op != CHAR_ && n2->op != INTEGER_)
            return 0;
        return n1->u.num == n2->u.num;
    case SET_:
        if (n2->op != SET_)
            return 0;
        return n1->u.num == n2->u.num;
    case LIST_:
        if (n2->op != LIST_)
            return 0;
        return equal_list_aux(n1->u.lis, n2->u.lis);
    default:
        return strcmp(GETSTRING(n1), GETSTRING(n2)) == 0;
    }
#endif
}

PRIVATE void equal_(void)
{
    TWOPARAMS("equal");
    BINARY(BOOLEAN_NEWNODE, equal_aux(stck, stck->next));
}

#ifdef CORRECT_INHAS_COMPARE
#define INHAS(PROCEDURE, NAME, AGGR, ELEM)                                     \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        int found = 0, error;                                                  \
        TWOPARAMS(NAME);                                                       \
        switch (AGGR->op) {                                                    \
        case SET_:                                                             \
            found = ((AGGR->u.set) & ((long_t)1 << ELEM->u.num)) > 0;          \
            break;                                                             \
        case STRING_: {                                                        \
            char* s;                                                           \
            for (s = AGGR->u.str; s && *s != '\0' && *s != ELEM->u.num; s++)   \
                ;                                                              \
            found = s && *s != '\0';                                           \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            Node* n = AGGR->u.lis;                                             \
            while (n != NULL && (Compare(n, ELEM, &error) || error))           \
                n = n->next;                                                   \
            found = n != NULL;                                                 \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        BINARY(BOOLEAN_NEWNODE, (long_t)found);                                \
    }
#else
#define INHAS(PROCEDURE, NAME, AGGR, ELEM)                                     \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        int found = 0;                                                         \
        TWOPARAMS(NAME);                                                       \
        switch (AGGR->op) {                                                    \
        case SET_:                                                             \
            found = ((AGGR->u.set) & ((long_t)1 << ELEM->u.num)) > 0;          \
            break;                                                             \
        case STRING_: {                                                        \
            char* s;                                                           \
            for (s = AGGR->u.str; *s != '\0' && *s != ELEM->u.num; s++)        \
                ;                                                              \
            found = *s != '\0';                                                \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            Node* n = AGGR->u.lis;                                             \
            while (n != NULL && n->u.num != ELEM->u.num)                       \
                n = n->next;                                                   \
            found = n != NULL;                                                 \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        BINARY(BOOLEAN_NEWNODE, (long_t)found);                                \
    }
#endif
INHAS(in_, "in", stck, stck->next)
INHAS(has_, "has", stck->next, stck)

#ifdef RUNTIME_CHECKS
#define OF_AT(PROCEDURE, NAME, AGGR, INDEX)                                    \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        if (INDEX->op != INTEGER_ || INDEX->u.num < 0)                         \
            execerror("non-negative integer", NAME);                           \
        switch (AGGR->op) {                                                    \
        case SET_: {                                                           \
            int i, indx = INDEX->u.num;                                        \
            CHECKEMPTYSET(AGGR->u.set, NAME);                                  \
            for (i = 0; i < SETSIZE; i++) {                                    \
                if (AGGR->u.set & ((long_t)1 << i)) {                          \
                    if (indx == 0) {                                           \
                        BINARY(INTEGER_NEWNODE, i);                            \
                        return;                                                \
                    }                                                          \
                    indx--;                                                    \
                }                                                              \
            }                                                                  \
            INDEXTOOLARGE(NAME);                                               \
            return;                                                            \
        }                                                                      \
        case STRING_:                                                          \
            if (strlen(AGGR->u.str) < (size_t)INDEX->u.num)                    \
                INDEXTOOLARGE(NAME);                                           \
            BINARY(CHAR_NEWNODE, (long_t)AGGR->u.str[INDEX->u.num]);           \
            return;                                                            \
        case LIST_: {                                                          \
            Node* n = AGGR->u.lis;                                             \
            int i = INDEX->u.num;                                              \
            CHECKEMPTYLIST(n, NAME);                                           \
            while (i > 0) {                                                    \
                if (n->next == NULL)                                           \
                    INDEXTOOLARGE(NAME);                                       \
                n = n->next;                                                   \
                i--;                                                           \
            }                                                                  \
            GBINARY(n->op, n->u);                                              \
            return;                                                            \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }
#else
#define OF_AT(PROCEDURE, NAME, AGGR, INDEX)                                    \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        switch (AGGR->op) {                                                    \
        case SET_: {                                                           \
            int i, indx = INDEX->u.num;                                        \
            for (i = 0; i < SETSIZE; i++) {                                    \
                if (AGGR->u.set & ((long_t)1 << i)) {                          \
                    if (indx == 0) {                                           \
                        BINARY(INTEGER_NEWNODE, i);                            \
                        return;                                                \
                    }                                                          \
                    indx--;                                                    \
                }                                                              \
            }                                                                  \
            return;                                                            \
        }                                                                      \
        case STRING_:                                                          \
            BINARY(CHAR_NEWNODE, (long_t)AGGR->u.str[INDEX->u.num]);           \
            return;                                                            \
        case LIST_: {                                                          \
            Node* n = AGGR->u.lis;                                             \
            int i = INDEX->u.num;                                              \
            while (i > 0) {                                                    \
                n = n->next;                                                   \
                i--;                                                           \
            }                                                                  \
            GBINARY(n->op, n->u);                                              \
            return;                                                            \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }
#endif
OF_AT(of_, "of", stck, stck->next)
OF_AT(at_, "at", stck->next, stck)

PRIVATE void choice_(void)
{
    THREEPARAMS("choice");
    if (stck->next->next->u.num)
        stck = newnode(stck->next->op, stck->next->u, stck->next->next->next);
    else
        stck = newnode(stck->op, stck->u, stck->next->next->next);
}

PRIVATE void case_(void)
{
    Node* n;
#ifdef CORRECT_CASE_COMPARE
    int error;
#endif

    TWOPARAMS("case");
    LIST("case");
    n = stck->u.lis;
    CHECKEMPTYLIST(n, "case");
    while (n->next != NULL && n->u.lis->u.num != stck->next->u.num) {
#ifdef CORRECT_CASE_COMPARE
        if (!Compare(n->u.lis, stck->next, &error) && !error)
            break;
#endif
        n = n->next;
    }
/*
    printf("case : now execute : ");
    writefactor(n->u.lis, stdout); printf("\n");
    stck = stck->next->next;
    exeterm(n->next != NULL ? n->u.lis->next : n->u.lis);
*/
    if (n->next != NULL) {
        stck = stck->next->next;
        exeterm(n->u.lis->next);
    } else {
        stck = stck->next;
        exeterm(n->u.lis);
    }
}

PRIVATE void opcase_(void)
{
    Node* n;
    ONEPARAM("opcase");
    LIST("opcase");
    n = stck->u.lis;
    CHECKEMPTYLIST(n, "opcase");
    while (n->next != NULL && n->op == LIST_ && n->u.lis->op != stck->next->op)
        n = n->next;
    CHECKLIST(n->op, "opcase");
    UNARY(LIST_NEWNODE, n->next != NULL ? n->u.lis->next : n->u.lis);
}

#ifdef RUNTIME_CHECKS
#define CONS_SWONS(PROCEDURE, NAME, AGGR, ELEM)                                \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        switch (AGGR->op) {                                                    \
        case LIST_:                                                            \
            BINARY(LIST_NEWNODE, newnode(ELEM->op, ELEM->u, AGGR->u.lis));     \
            break;                                                             \
        case SET_:                                                             \
            CHECKSETMEMBER(ELEM, NAME);                                        \
            BINARY(SET_NEWNODE, AGGR->u.set | ((long_t)1 << ELEM->u.num));     \
            break;                                                             \
        case STRING_: {                                                        \
            char* s;                                                           \
            if (ELEM->op != CHAR_)                                             \
                execerror("character", NAME);                                  \
            s = (char*)malloc(strlen(AGGR->u.str) + 2);                        \
            s[0] = (char)ELEM->u.num;                                          \
            strcpy(s + 1, AGGR->u.str);                                        \
            BINARY(STRING_NEWNODE, s);                                         \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }
#else
#define CONS_SWONS(PROCEDURE, NAME, AGGR, ELEM)                                \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        switch (AGGR->op) {                                                    \
        case LIST_:                                                            \
            BINARY(LIST_NEWNODE, newnode(ELEM->op, ELEM->u, AGGR->u.lis));     \
            break;                                                             \
        case SET_:                                                             \
            BINARY(SET_NEWNODE, AGGR->u.set | ((long_t)1 << ELEM->u.num));     \
            break;                                                             \
        case STRING_: {                                                        \
            char* s;                                                           \
            s = (char*)malloc(strlen(AGGR->u.str) + 2);                        \
            s[0] = ELEM->u.num;                                                \
            strcpy(s + 1, AGGR->u.str);                                        \
            BINARY(STRING_NEWNODE, s);                                         \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }
#endif
CONS_SWONS(cons_, "cons", stck, stck->next)
CONS_SWONS(swons_, "swons", stck->next, stck)

PRIVATE void drop_(void)
{
    int n = stck->u.num;
    TWOPARAMS("drop");
    switch (stck->next->op) {
    case SET_: {
        int i;
        long_t result = 0;
        for (i = 0; i < SETSIZE; i++)
            if (stck->next->u.set & ((long_t)1 << i)) {
                if (n < 1)
                    result |= ((long_t)1 << i);
                else
                    n--;
            }
        BINARY(SET_NEWNODE, result);
        return;
    }
    case STRING_: {
        char* result = stck->next->u.str;
        while (n-- > 0 && *result != '\0')
            ++result;
        BINARY(STRING_NEWNODE, result);
        return;
    }
    case LIST_: {
        Node* result = stck->next->u.lis;
        while (n-- > 0 && result != NULL)
            result = result->next;
        BINARY(LIST_NEWNODE, result);
        return;
    }
    default:
        BADAGGREGATE("drop");
    }
}

PRIVATE void take_(void)
{
    int n = stck->u.num;

    TWOPARAMS("take");
    switch (stck->next->op) {
    case SET_: {
        int i;
        long_t result = 0;
        for (i = 0; i < SETSIZE; i++)
            if (stck->next->u.set & ((long_t)1 << i)) {
                if (n > 0) {
                    --n;
                    result |= ((long_t)1 << i);
                } else
                    break;
            }
        BINARY(SET_NEWNODE, result);
        return;
    }
    case STRING_: {
        int i;
        char *old, *p, *result;
        i = stck->u.num;
        old = stck->next->u.str;
        POP(stck);
        /* do not swap the order of the next two statements ! ! ! */
        if (i < 0)
            i = 0;
        if ((size_t)i >= strlen(old))
            return; /* the old string unchanged */
        p = result = (char*)malloc(i + 1);
        while (i-- > 0)
            *p++ = *old++;
        *p = 0;
        UNARY(STRING_NEWNODE, result);
        return;
    }
    case LIST_: {
        int i = stck->u.num;
        if (i < 1) {
            BINARY(LIST_NEWNODE, NULL);
            return;
        } /* null string */
        dump1 = newnode(LIST_, stck->next->u, dump1); /* old  */
        dump2 = LIST_NEWNODE(0L, dump2); /* head */
        dump3 = LIST_NEWNODE(0L, dump3); /* last */
        while (DMP1 != NULL && i-- > 0) {
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(DMP1->op, DMP1->u, NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                DMP3->next = NEWNODE(DMP1->op, DMP1->u, NULL);
                DMP3 = DMP3->next;
            }
            DMP1 = DMP1->next;
        }
        DMP3->next = NULL;
        BINARY(LIST_NEWNODE, DMP2);
        POP(dump1);
        POP(dump2);
        POP(dump3);
        return;
    }
    default:
        BADAGGREGATE("take");
    }
}

PRIVATE void concat_(void)
{
    TWOPARAMS("concat");
    SAME2TYPES("concat");
    switch (stck->op) {
    case SET_:
        BINARY(SET_NEWNODE, stck->next->u.set | stck->u.set);
        return;
    case STRING_: {
        char *s, *p;
        s = p = (char*)malloc(
            strlen(stck->next->u.str) + strlen(stck->u.str) + 1);
        strcpy(s, stck->next->u.str);
        strcat(s, stck->u.str);
        BINARY(STRING_NEWNODE, s);
        return;
    }
    case LIST_:
        if (stck->next->u.lis == NULL) {
            BINARY(LIST_NEWNODE, stck->u.lis);
            return;
        }
        dump1 = LIST_NEWNODE(stck->next->u.lis, dump1); /* old  */
        dump2 = LIST_NEWNODE(0L, dump2); /* head */
        dump3 = LIST_NEWNODE(0L, dump3); /* last */
        while (DMP1 != NULL) {
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(DMP1->op, DMP1->u, NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                DMP3->next = NEWNODE(DMP1->op, DMP1->u, NULL);
                DMP3 = DMP3->next;
            };
            DMP1 = DMP1->next;
        }
        DMP3->next = stck->u.lis;
        BINARY(LIST_NEWNODE, DMP2);
        POP(dump1);
        POP(dump2);
        POP(dump3);
        return;
    default:
        BADAGGREGATE("concat");
    };
}

PRIVATE void enconcat_(void)
{
    THREEPARAMS("enconcat");
    SAME2TYPES("enconcat");
    swapd_();
    cons_();
    concat_();
}

PRIVATE void null_(void)
{
    ONEPARAM("null");
    switch (stck->op) {
    case STRING_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(*(stck->u.str) == '\0'));
        break;
    case FLOAT_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(stck->u.dbl == 0.0));
        break;
    case FILE_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(stck->u.fil == NULL));
        break;
    case LIST_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!stck->u.lis));
        break;
    case SET_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!stck->u.set));
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!stck->u.num));
        break;
    default:
        BADDATA("null");
    }
}

PRIVATE void not_(void)
{
    ONEPARAM("not");
    switch (stck->op) {
    case SET_:
        UNARY(SET_NEWNODE, ~stck->u.set);
        break;
#ifndef ONLY_LOGICAL_NOT
    case STRING_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(*(stck->u.str) != '\0'));
        break;
    case LIST_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!stck->u.lis));
        break;
    case CHAR_:
    case INTEGER_:
#endif
    case BOOLEAN_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!stck->u.num));
        break;
#ifdef NOT_ALSO_FOR_FLOAT
    case FLOAT_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!stck->u.dbl));
        break;
#endif
#ifdef NOT_ALSO_FOR_FILE
    case FILE_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(stck->u.fil != NULL));
        break;
#endif
    default:
        BADDATA("not");
    }
}

PRIVATE void size_(void)
{
    long_t siz = 0;
    ONEPARAM("size");
    switch (stck->op) {
    case SET_: {
        int i;
        for (i = 0; i < SETSIZE; i++)
            if (stck->u.set & ((long_t)1 << i))
                siz++;
        break;
    }
    case STRING_:
        siz = strlen(stck->u.str);
        break;
    case LIST_: {
        Node* e = stck->u.lis;
        while (e != NULL) {
            e = e->next;
            siz++;
        };
        break;
    }
    default:
        BADAGGREGATE("size");
    }
    UNARY(INTEGER_NEWNODE, siz);
}

PRIVATE void small_(void)
{
    int sml = 0;
    ONEPARAM("small");
    switch (stck->op) {
    case BOOLEAN_:
    case INTEGER_:
        sml = stck->u.num < 2;
        break;
    case SET_:
        if (stck->u.set == 0)
            sml = 1;
        else {
            int i = 0;
            while (!(stck->u.set & ((long_t)1 << i)))
                i++;
            D(printf("small: first member found is %d\n", i);)
            sml = (stck->u.set & ~((long_t)1 << i)) == 0;
        }
        break;
    case STRING_:
        sml = stck->u.str[0] == '\0' || stck->u.str[1] == '\0';
        break;
    case LIST_:
        sml = stck->u.lis == NULL || stck->u.lis->next == NULL;
        break;
    default:
        BADDATA("small");
    }
    UNARY(BOOLEAN_NEWNODE, sml);
}

#define TYPE(PROCEDURE, NAME, REL, TYP)                                        \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        UNARY(BOOLEAN_NEWNODE, (long_t)(stck->op REL TYP));                    \
    }
TYPE(integer_, "integer", ==, INTEGER_)
TYPE(char_, "char", ==, CHAR_)
TYPE(logical_, "logical", ==, BOOLEAN_)
TYPE(string_, "string", ==, STRING_)
TYPE(set_, "set", ==, SET_)
TYPE(list_, "list", ==, LIST_)
TYPE(leaf_, "leaf", !=, LIST_)
TYPE(float_, "float", ==, FLOAT_)
TYPE(file_, "file", ==, FILE_)
TYPE(user_, "user", ==, USR_)

#define USETOP(PROCEDURE, NAME, TYPE, BODY)                                    \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        TYPE(NAME);                                                            \
        BODY;                                                                  \
        POP(stck);                                                             \
    }
USETOP(put_, "put", ONEPARAM, writefactor(stck, stdout); printf(" "))
USETOP(putch_, "putch", NUMERICTYPE, printf("%c", (char)stck->u.num))
USETOP(putchars_, "putchars", STRING, printf("%s", stck->u.str))
USETOP(setecho_, "setecho", NUMERICTYPE, echoflag = stck->u.num)
USETOP(setautoput_, "setautoput", NUMERICTYPE, autoput = stck->u.num)
USETOP(setundeferror_, "setundeferror", NUMERICTYPE, undeferror = stck->u.num)
USETOP(settracegc_, "settracegc", NUMERICTYPE, tracegc = stck->u.num)
USETOP(srand_, "srand", INTEGER, my_srand((unsigned)stck->u.num))
USETOP(include_, "include", STRING, doinclude(stck->u.str))
USETOP(system_, "system", STRING, (void)system(stck->u.str))

PRIVATE void undefs_(void)
{
    Entry* i = symtabindex;
    dump1 = LIST_NEWNODE(NULL, dump1);
    while (i != symtab) {
        --i;
        if ((i->name[0] != 0) && (i->name[0] != '_') && (i->u.body == NULL))
            DMP1 = STRING_NEWNODE(i->name, DMP1);
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(dump1);
}

PRIVATE void argv_(void)
{
    int i;
    dump1 = LIST_NEWNODE(NULL, dump1);
    for (i = g_argc - 1; i >= 0; i--) {
        DMP1 = STRING_NEWNODE(g_argv[i], DMP1);
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(dump1);
    return;
}

PRIVATE void get_(void)
{
    getsym();
    readfactor(0);
}

PUBLIC void dummy_(void) { /* never called */ }

#define HELP(PROCEDURE, REL)                                                   \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        Entry* i = symtabindex;                                                \
        int column = 0;                                                        \
        int name_length;                                                       \
        while (i != symtab)                                                    \
            if ((--i)->name[0] REL '_' && !i->is_local) {                      \
                name_length = strlen(i->name) + 1;                             \
                if (column + name_length > 72) {                               \
                    printf("\n");                                              \
                    column = 0;                                                \
                }                                                              \
                printf("%s ", i->name);                                        \
                column += name_length;                                         \
            }                                                                  \
        printf("\n");                                                          \
    }
HELP(help1_, !=)
HELP(h_help1_, ==)

/* - - - - -   C O M B I N A T O R S   - - - - - */

#ifdef TRACE
PUBLIC void printfactor(Node* n, FILE* stm)
{
    switch (n->op) {
    case BOOLEAN_:
        fprintf(stm, "type boolean");
        return;
    case INTEGER_:
        fprintf(stm, "type integer");
        return;
    case FLOAT_:
        fprintf(stm, "type float");
        return;
    case SET_:
        fprintf(stm, "type set");
        return;
    case CHAR_:
        fprintf(stm, "type char");
        return;
    case STRING_:
        fprintf(stm, "type string");
        return;
    case LIST_:
        fprintf(stm, "type list");
        return;
    case USR_:
        fprintf(stm, n->u.ent->name);
        return;
    case FILE_:
        fprintf(stm, "type file");
        return;
    default:
        fprintf(stm, "%s", symtab[(int)n->op].name);
        return;
    }
}
#endif

#ifdef TRACK_USED_SYMBOLS
static void report_symbols(void)
{
    Entry* n;

    for (n = symtab; n->name; n++)
        if (n->is_used)
            fprintf(stderr, "%s\n", n->name);
}
#endif

#ifdef STATS
static double calls, opers;

static void report_stats(void)
{
    fprintf(stderr, "%.0f calls to joy interpreter\n", calls);
    fprintf(stderr, "%.0f operations executed\n", opers);
}
#endif

PUBLIC void exeterm(Node* n)
{
    Node* stepper;
#ifdef TRACK_USED_SYMBOLS
    static int first;

    if (!first) {
        first = 1;
        atexit(report_symbols);
    }
#endif
start:
    if (n == NULL)
        return;
#ifdef STATS
    if (++calls == 1)
	atexit(report_stats);
#endif
    conts = LIST_NEWNODE(n, conts);
    while (conts->u.lis != NULL) {
#ifdef ENABLE_TRACEGC
        if (tracegc > 5) {
            printf("exeterm1: %p ", (void*)conts->u.lis);
            printnode(conts->u.lis);
        }
#endif
        stepper = conts->u.lis;
        conts->u.lis = conts->u.lis->next;
#ifdef TRACE
        printfactor(stepper, stdout);
        printf(" . ");
        writeterm(stck, stdout);
        printf("\n");
#endif
#ifdef STATS
	++opers;
#endif
        switch (stepper->op) {
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case FLOAT_:
        case SET_:
        case STRING_:
        case LIST_:
        case FILE_:
            stck = newnode(stepper->op, stepper->u, stck);
            break;
        case USR_:
            if (stepper->u.ent->u.body == NULL && undeferror)
                execerror("definition", stepper->u.ent->name);
            if (stepper->next == NULL) {
                POP(conts);
                n = stepper->u.ent->u.body;
                goto start;
            } else
                exeterm(stepper->u.ent->u.body);
            break;
        case COPIED_:
        case ILLEGAL_:
            printf("exeterm: attempting to execute bad node\n");
#ifdef ENABLE_TRACEGC
            printnode(stepper);
#endif
            break;
        default:
            D(printf("trying to do ");)
            D(writefactor(stepper, stdout);)
#ifdef TRACK_USED_SYMBOLS
            symtab[(int)stepper->op].is_used = 1;
#endif
            (*(stepper->u.proc))();
            break;
        }
#ifdef ENABLE_TRACEGC
        if (tracegc > 5) {
            printf("exeterm2: %p ", (void*)stepper);
            printnode(stepper);
        }
#endif
    }
    POP(conts);
    D(printf("after execution, stck is:\n");)
    D(writeterm(stck, stdout);)
    D(printf("\n");)
}

PRIVATE void x_(void)
{
    ONEPARAM("x");
    ONEQUOTE("x");
    exeterm(stck->u.lis);
}

PRIVATE void i_(void)
{
    ONEPARAM("i");
    ONEQUOTE("i");
    SAVESTACK;
    POP(stck);
    exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void dip_(void)
{
    TWOPARAMS("dip");
    ONEQUOTE("dip");
    SAVESTACK;
    stck = stck->next->next;
    exeterm(SAVED1->u.lis);
    GNULLARY(SAVED2->op, SAVED2->u);
    POP(dump);
}

#define N_ARY(PROCEDURE, NAME, PARAMCOUNT, TOP)                                \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        PARAMCOUNT(NAME);                                                      \
        ONEQUOTE(NAME);                                                        \
        SAVESTACK;                                                             \
        POP(stck);                                                             \
        exeterm(SAVED1->u.lis);                                                \
        if (stck == NULL)                                                      \
            execerror("value to push", NAME);                                  \
        stck = newnode(stck->op, stck->u, TOP);                                \
        POP(dump);                                                             \
    }
N_ARY(nullary_, "nullary", ONEPARAM, SAVED2)
N_ARY(unary_, "unary", TWOPARAMS, SAVED3)
N_ARY(binary_, "binary", THREEPARAMS, SAVED4)
N_ARY(ternary_, "ternary", FOURPARAMS, SAVED5)

/*
PRIVATE void nullary_(void)
{
    ONEPARAM("nullary");
    SAVESTACK;
    POP(stck);
    exeterm(SAVED1->u.lis);
    stck->next = SAVED2;
    POP(dump);
}
*/

PRIVATE void times_(void)
{
    int i, n;
    TWOPARAMS("times");
    ONEQUOTE("times");
    INTEGER2("times");
    SAVESTACK;
    stck = stck->next->next;
    n = SAVED2->u.num;
    for (i = 1; i <= n; i++)
        exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void infra_(void)
{
    TWOPARAMS("infra");
    ONEQUOTE("infra");
    LIST2("infra");
    SAVESTACK;
    stck = SAVED2->u.lis;
    exeterm(SAVED1->u.lis);
    stck = LIST_NEWNODE(stck, SAVED3);
    POP(dump);
}

PRIVATE void app1_(void)
{
    TWOPARAMS("app1");
    ONEQUOTE("app1");
    SAVESTACK;
    POP(stck);
    exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void cleave_(void)
{ /*  X [P1] [P2] cleave ==>  X1 X2	*/
    THREEPARAMS("cleave");
    TWOQUOTES("cleave");
    SAVESTACK;
    stck = SAVED3;
    exeterm(SAVED2->u.lis); /* [P1]		*/
    dump1 = newnode(stck->op, stck->u, dump1); /*  X1		*/
    stck = SAVED3;
    exeterm(SAVED1->u.lis); /* [P2]		*/
    dump1 = newnode(stck->op, stck->u, dump1); /*  X2		*/
    stck = dump1;
    dump1 = dump1->next->next;
    stck->next->next = SAVED4;
    POP(dump);
}

PRIVATE void app11_(void)
{
    THREEPARAMS("app11");
    ONEQUOTE("app11");
    app1_();
    stck->next = stck->next->next;
}

PRIVATE void unary2_(void)
{ /*   Y  Z  [P]  unary2     ==>  Y'  Z'  */
    THREEPARAMS("unary2");
    ONEQUOTE("unary2");
    SAVESTACK;
    stck = SAVED2->next; /* just Y on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(Y) */
    stck = newnode(SAVED2->op, SAVED2->u, SAVED3->next); /* just Z on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(Z) */
    stck = dump1;
    dump1 = dump1->next->next;
    stck->next->next = SAVED4;
    POP(dump);
}

PRIVATE void unary3_(void)
{ /*  X Y Z [P]  unary3    ==>  X' Y' Z'	*/
    FOURPARAMS("unary3");
    ONEQUOTE("unary3");
    SAVESTACK;
    stck = SAVED3->next; /* just X on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(X) */
    stck = newnode(SAVED3->op, SAVED3->u, SAVED4->next); /* just Y on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(Y) */
    stck = newnode(SAVED2->op, SAVED2->u, SAVED4->next); /* just Z on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(Z) */
    stck = dump1;
    dump1 = dump1->next->next->next;
    stck->next->next->next = SAVED5;
    POP(dump);
}

PRIVATE void unary4_(void)
{ /*  X Y Z W [P]  unary4    ==>  X' Y' Z' W'	*/
    FIVEPARAMS("unary4");
    ONEQUOTE("unary4");
    SAVESTACK;
    stck = SAVED4->next; /* just X on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save p(X) */
    stck = newnode(SAVED4->op, SAVED4->u, SAVED5->next); /* just Y on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(Y) */
    stck = newnode(SAVED3->op, SAVED3->u, SAVED5->next); /* just Z on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(Z) */
    stck = newnode(SAVED2->op, SAVED2->u, SAVED5->next); /* just W on top */
    exeterm(SAVED1->u.lis); /* execute P */
    dump1 = newnode(stck->op, stck->u, dump1); /* save P(W) */
    stck = dump1;
    dump1 = dump1->next->next->next->next;
    stck->next->next->next->next = SAVED6;
    POP(dump);
}

PRIVATE void app12_(void)
{
    /*   X  Y  Z  [P]  app12  */
    THREEPARAMS("app12");
    unary2_();
    stck->next->next = stck->next->next->next; /* delete X */
}

PRIVATE void map_(void)
{
    TWOPARAMS("map");
    ONEQUOTE("map");
    SAVESTACK;
    switch (SAVED2->op) {
    case LIST_: {
        dump1 = newnode(LIST_, SAVED2->u, dump1); /* step old */
        dump2 = LIST_NEWNODE(0L, dump2); /* head new */
        dump3 = LIST_NEWNODE(0L, dump3); /* last new */
        while (DMP1 != NULL) {
            stck = newnode(DMP1->op, DMP1->u, SAVED3);
            exeterm(SAVED1->u.lis);
            D(printf("map: "); writefactor(stck, stdout); printf("\n");)
#if defined(RUNTIME_CHECKS)
            if (stck == NULL)
                execerror("non-empty stack", "map");
#endif
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(stck->op, stck->u, NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                DMP3->next = NEWNODE(stck->op, stck->u, NULL);
                DMP3 = DMP3->next;
            }
            DMP1 = DMP1->next;
        }
        stck = LIST_NEWNODE(DMP2, SAVED3);
        POP(dump3);
        POP(dump2);
        POP(dump1);
        break;
    }
    case STRING_: {
        char *s, *resultstring;
        int j = 0;
        resultstring = (char*)malloc(strlen(SAVED2->u.str) + 1);
        for (s = SAVED2->u.str; *s != '\0'; s++) {
            stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(SAVED1->u.lis);
            resultstring[j++] = stck->u.num;
        }
        resultstring[j] = '\0';
        stck = STRING_NEWNODE(resultstring, SAVED3);
        break;
    }
    case SET_: {
        int i;
        long_t resultset = 0;
        for (i = 0; i < SETSIZE; i++)
            if (SAVED2->u.set & ((long_t)1 << i)) {
                stck = INTEGER_NEWNODE(i, SAVED3);
                exeterm(SAVED1->u.lis);
                resultset |= ((long_t)1 << stck->u.num);
            }
        stck = SET_NEWNODE(resultset, SAVED3);
        break;
    }
    default:
        BADAGGREGATE("map");
    }
    POP(dump);
}

PRIVATE void step_(void)
{
    TWOPARAMS("step");
    ONEQUOTE("step");
    SAVESTACK;
    stck = stck->next->next;
    switch (SAVED2->op) {
    case LIST_: {
        dump1 = newnode(LIST_, SAVED2->u, dump1);
        while (DMP1 != NULL) {
            GNULLARY(DMP1->op, DMP1->u);
            exeterm(SAVED1->u.lis);
            DMP1 = DMP1->next;
        }
        POP(dump1);
        break;
    }
    case STRING_: {
        char* s;
        for (s = SAVED2->u.str; *s != '\0'; s++) {
            stck = CHAR_NEWNODE((long_t)*s, stck);
            exeterm(SAVED1->u.lis);
        }
        break;
    }
    case SET_: {
        int i;
        for (i = 0; i < SETSIZE; i++)
            if (SAVED2->u.set & ((long_t)1 << i)) {
                stck = INTEGER_NEWNODE(i, stck);
                exeterm(SAVED1->u.lis);
            }
        break;
    }
    default:
        BADAGGREGATE("step");
    }
    POP(dump);
}

PRIVATE void fold_(void)
{
    THREEPARAMS("fold");
    swapd_();
    step_();
}

PRIVATE void cond_(void)
{
#ifdef RUNTIME_CHECKS
    Node* my_dump;
#endif
    int result = 0;
    ONEPARAM("cond");
    LIST("cond");
    CHECKEMPTYLIST(stck->u.lis, "cond");
/* must check for QUOTES in list */
#ifdef RUNTIME_CHECKS
    for (my_dump = stck->u.lis; my_dump->next; my_dump = my_dump->next)
        CHECKLIST(my_dump->u.lis->op, "cond");
#endif
    SAVESTACK;
    dump1 = newnode(LIST_, stck->u, dump1);
    while (result == 0 && DMP1 != NULL && DMP1->next != NULL) {
        stck = SAVED2;
        exeterm(DMP1->u.lis->u.lis);
        result = stck->u.num;
        if (!result)
            DMP1 = DMP1->next;
    }
    stck = SAVED2;
    if (result)
        exeterm(DMP1->u.lis->next);
    else
        exeterm(DMP1->u.lis); /* default */
    POP(dump1);
    POP(dump);
}

#define IF_TYPE(PROCEDURE, NAME, TYP)                                          \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        TWOQUOTES(NAME);                                                       \
        SAVESTACK;                                                             \
        stck = SAVED3;                                                         \
        exeterm(stck->op == TYP ? SAVED2->u.lis : SAVED1->u.lis);              \
        POP(dump);                                                             \
    }
IF_TYPE(ifinteger_, "ifinteger", INTEGER_)
IF_TYPE(ifchar_, "ifchar", CHAR_)
IF_TYPE(iflogical_, "iflogical", BOOLEAN_)
IF_TYPE(ifstring_, "ifstring", STRING_)
IF_TYPE(ifset_, "ifset", SET_)
IF_TYPE(iffloat_, "iffloat", FLOAT_)
IF_TYPE(iffile_, "iffile", FILE_)
IF_TYPE(iflist_, "iflist", LIST_)

PRIVATE void filter_(void)
{
    TWOPARAMS("filter");
    ONEQUOTE("filter");
    SAVESTACK;
    switch (SAVED2->op) {
    case SET_: {
        int j;
        long_t resultset = 0;
        for (j = 0; j < SETSIZE; j++) {
            if (SAVED2->u.set & ((long_t)1 << j)) {
                stck = INTEGER_NEWNODE(j, SAVED3);
                exeterm(SAVED1->u.lis);
                if (stck->u.num)
                    resultset |= ((long_t)1 << j);
            }
        }
        stck = SET_NEWNODE(resultset, SAVED3);
        break;
    }
    case STRING_: {
        char *s, *resultstring;
        int j = 0;
        resultstring = (char*)malloc(strlen(SAVED2->u.str) + 1);
        for (s = SAVED2->u.str; *s != '\0'; s++) {
            stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(SAVED1->u.lis);
            if (stck->u.num)
                resultstring[j++] = *s;
        }
        resultstring[j] = '\0';
        stck = STRING_NEWNODE(resultstring, SAVED3);
        break;
    }
    case LIST_: {
        dump1 = newnode(LIST_, SAVED2->u, dump1); /* step old */
        dump2 = LIST_NEWNODE(0L, dump2); /* head new */
        dump3 = LIST_NEWNODE(0L, dump3); /* last new */
        while (DMP1 != NULL) {
            stck = newnode(DMP1->op, DMP1->u, SAVED3);
            exeterm(SAVED1->u.lis);
            D(printf("filter: "); writefactor(stck, stdout); printf("\n");)
            if (stck->u.num) /* test */
            {
                if (DMP2 == NULL) /* first */
                {
                    DMP2 = NEWNODE(DMP1->op, DMP1->u, NULL);
                    DMP3 = DMP2;
                } else /* further */
                {
                    DMP3->next = NEWNODE(DMP1->op, DMP1->u, NULL);
                    DMP3 = DMP3->next;
                }
            }
            DMP1 = DMP1->next;
        }
        stck = LIST_NEWNODE(DMP2, SAVED3);
        POP(dump3);
        POP(dump2);
        POP(dump1);
        break;
    }
    default:
        BADAGGREGATE("filter");
    }
    POP(dump);
}

PRIVATE void split_(void)
{
    TWOPARAMS("split");
    ONEQUOTE("split");
    SAVESTACK;
    switch (SAVED2->op) {
    case SET_: {
        int j;
        long_t yes_set = 0, no_set = 0;
        for (j = 0; j < SETSIZE; j++) {
            if (SAVED2->u.set & ((long_t)1 << j)) {
                stck = INTEGER_NEWNODE(j, SAVED3);
                exeterm(SAVED1->u.lis);
                if (stck->u.num)
                    yes_set |= ((long_t)1 << j);
                else
                    no_set |= ((long_t)1 << j);
            }
        }
        stck = SET_NEWNODE(yes_set, SAVED3);
        NULLARY(SET_NEWNODE, no_set);
        break;
    }
    case STRING_: {
        char *s, *yesstring, *nostring;
        int yesptr = 0, noptr = 0;
        yesstring = (char*)malloc(strlen(SAVED2->u.str) + 1);
        nostring = (char*)malloc(strlen(SAVED2->u.str) + 1);
        for (s = SAVED2->u.str; *s != '\0'; s++) {
            stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(SAVED1->u.lis);
            if (stck->u.num)
                yesstring[yesptr++] = *s;
            else
                nostring[noptr++] = *s;
        }
        yesstring[yesptr] = '\0';
        nostring[noptr] = '\0';
        stck = STRING_NEWNODE(yesstring, SAVED3);
        NULLARY(STRING_NEWNODE, nostring);
        break;
    }
    case LIST_: {
        dump1 = newnode(LIST_, SAVED2->u, dump1); /* step old */
        dump2 = LIST_NEWNODE(0L, dump2); /* head true */
        dump3 = LIST_NEWNODE(0L, dump3); /* last true */
        dump4 = LIST_NEWNODE(0L, dump4); /* head false */
        dump5 = LIST_NEWNODE(0L, dump5); /* last false */
        while (DMP1 != NULL) {
            stck = newnode(DMP1->op, DMP1->u, SAVED3);
            exeterm(SAVED1->u.lis);
            D(printf("split: "); writefactor(stck, stdout); printf("\n");)
            if (stck->u.num) /* pass */
                if (DMP2 == NULL) /* first */
                {
                    DMP2 = NEWNODE(DMP1->op, DMP1->u, NULL);
                    DMP3 = DMP2;
                } else /* further */
                {
                    DMP3->next = NEWNODE(DMP1->op, DMP1->u, NULL);
                    DMP3 = DMP3->next;
                }
            else /* fail */
                if (DMP4 == NULL) /* first */
            {
                DMP4 = NEWNODE(DMP1->op, DMP1->u, NULL);
                DMP5 = DMP4;
            } else /* further */
            {
                DMP5->next = NEWNODE(DMP1->op, DMP1->u, NULL);
                DMP5 = DMP5->next;
            }
            DMP1 = DMP1->next;
        }
        stck = LIST_NEWNODE(DMP2, SAVED3);
        NULLARY(LIST_NEWNODE, DMP4);
        POP(dump5);
        POP(dump4);
        POP(dump3);
        POP(dump2);
        POP(dump1);
        break;
    }
    default:
        BADAGGREGATE("split");
    }
    POP(dump);
}

#define SOMEALL(PROCEDURE, NAME, INITIAL)                                      \
    PRIVATE void PROCEDURE(void)                                               \
    {                                                                          \
        int result = INITIAL;                                                  \
        TWOPARAMS(NAME);                                                       \
        ONEQUOTE(NAME);                                                        \
        SAVESTACK;                                                             \
        switch (SAVED2->op) {                                                  \
        case SET_: {                                                           \
            int j;                                                             \
            for (j = 0; j < SETSIZE && result == INITIAL; j++) {               \
                if (SAVED2->u.set & ((long_t)1 << j)) {                        \
                    stck = INTEGER_NEWNODE(j, SAVED3);                         \
                    exeterm(SAVED1->u.lis);                                    \
                    if (stck->u.num != INITIAL)                                \
                        result = 1 - INITIAL;                                  \
                }                                                              \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        case STRING_: {                                                        \
            char* s;                                                           \
            for (s = SAVED2->u.str; *s != '\0' && result == INITIAL; s++) {    \
                stck = CHAR_NEWNODE((long_t)*s, SAVED3);                       \
                exeterm(SAVED1->u.lis);                                        \
                if (stck->u.num != INITIAL)                                    \
                    result = 1 - INITIAL;                                      \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            dump1 = newnode(LIST_, SAVED2->u, dump1);                          \
            while (DMP1 != NULL && result == INITIAL) {                        \
                stck = newnode(DMP1->op, DMP1->u, SAVED3);                     \
                exeterm(SAVED1->u.lis);                                        \
                if (stck->u.num != INITIAL)                                    \
                    result = 1 - INITIAL;                                      \
                DMP1 = DMP1->next;                                             \
            }                                                                  \
            POP(dump1);                                                        \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        stck = BOOLEAN_NEWNODE(result, SAVED3);                                \
        POP(dump);                                                             \
    }
SOMEALL(some_, "some", 0L)
SOMEALL(all_, "all", 1L)

PRIVATE void primrec_(void)
{
    int n = 0;
    int i;
    THREEPARAMS("primrec");
    TWOQUOTES("primrec");
    SAVESTACK;
    stck = stck->next->next->next;
    switch (SAVED3->op) {
    case LIST_: {
        dump1 = newnode(LIST_, SAVED3->u, dump1);
        while (DMP1 != NULL) {
            stck = newnode(DMP1->op, DMP1->u, stck);
            DMP1 = DMP1->next;
            n++;
        }
        POP(dump1);
        break;
    }
    case STRING_: {
        char* s;
        for (s = SAVED3->u.str; *s != '\0'; s++) {
            stck = CHAR_NEWNODE((long_t)*s, stck);
            n++;
        }
        break;
    }
    case SET_: {
        int j;
        long_t set = SAVED3->u.set;
        for (j = 0; j < SETSIZE; j++)
            if (set & ((long_t)1 << j)) {
                stck = INTEGER_NEWNODE(j, stck);
                n++;
            }
        break;
    }
    case INTEGER_: {
        long_t j;
        for (j = SAVED3->u.num; j > 0; j--) {
            stck = INTEGER_NEWNODE(j, stck);
            n++;
        }
        break;
    }
    default:
        BADDATA("primrec");
    }
    exeterm(SAVED2->u.lis);
    for (i = 1; i <= n; i++)
        exeterm(SAVED1->u.lis);
    POP(dump);
}

PRIVATE void tailrecaux(void)
{
    int result;
tailrec:
    dump1 = LIST_NEWNODE(stck, dump1);
    exeterm(SAVED3->u.lis);
    result = stck->u.num;
    stck = DMP1;
    POP(dump1);
    if (result)
        exeterm(SAVED2->u.lis);
    else {
        exeterm(SAVED1->u.lis);
        goto tailrec;
    }
}

PRIVATE void tailrec_(void)
{
    THREEPARAMS("tailrec");
    THREEQUOTES("tailrec");
    SAVESTACK;
    stck = SAVED4;
    tailrecaux();
    POP(dump);
}

PRIVATE void construct_(void)
{ /* [P] [[P1] [P2] ..] -> X1 X2 ..	*/
    TWOPARAMS("construct");
    TWOQUOTES("construct");
    SAVESTACK;
    stck = SAVED3; /* pop progs		*/
    dump1 = LIST_NEWNODE(dump2, dump1); /* save dump2		*/
    dump2 = stck; /* save old stack	*/
    exeterm(SAVED2->u.lis); /* [P]			*/
    dump3 = LIST_NEWNODE(stck, dump3); /* save current stack	*/
    dump4 = newnode(LIST_, SAVED1->u, dump4); /* step [..]	*/
    while (DMP4 != NULL) {
        stck = DMP3; /* restore new stack	*/
        exeterm(DMP4->u.lis);
        dump2 = newnode(stck->op, stck->u, dump2); /* result	*/
        DMP4 = DMP4->next;
    }
    POP(dump4);
    POP(dump3);
    stck = dump2;
    dump2 = dump1->u.lis; /* restore dump2	*/
    POP(dump1);
    POP(dump);
}

PRIVATE void branch_(void)
{
    THREEPARAMS("branch");
    TWOQUOTES("branch");
    SAVESTACK;
    stck = SAVED4;
    exeterm(SAVED3->u.num ? SAVED2->u.lis : SAVED1->u.lis);
    POP(dump);
}

PRIVATE void while_(void)
{
    TWOPARAMS("while");
    TWOQUOTES("while");
    SAVESTACK;
    do {
        stck = SAVED3;
        exeterm(SAVED2->u.lis); /* TEST */
        if (!stck->u.num)
            break;
        stck = SAVED3;
        exeterm(SAVED1->u.lis); /* DO */
        SAVED3 = stck;
    } while (1);
    stck = SAVED3;
    POP(dump);
}

PRIVATE void ifte_(void)
{
    int result;
    THREEPARAMS("ifte");
    THREEQUOTES("ifte");
    SAVESTACK;
    stck = SAVED4;
    exeterm(SAVED3->u.lis);
    result = stck->u.num;
    stck = SAVED4;
    exeterm(result ? SAVED2->u.lis : SAVED1->u.lis);
    POP(dump);
}

PRIVATE void condlinrecaux(void)
{
    int result = 0;
    dump1 = newnode(LIST_, SAVED1->u, dump1);
    dump2 = LIST_NEWNODE(stck, dump2);
    while (result == 0 && DMP1 != NULL && DMP1->next != NULL) {
        stck = DMP2;
        exeterm(DMP1->u.lis->u.lis);
        result = stck->u.num;
        if (!result)
            DMP1 = DMP1->next;
    }
    stck = DMP2;
    if (result) {
        exeterm(DMP1->u.lis->next->u.lis);
        if (DMP1->u.lis->next->next != NULL) {
            condlinrecaux();
            exeterm(DMP1->u.lis->next->next->u.lis);
        }
    } else {
        exeterm(DMP1->u.lis->u.lis);
        if (DMP1->u.lis->next != NULL) {
            condlinrecaux();
            exeterm(DMP1->u.lis->next->u.lis);
        }
    }
    POP(dump2);
    POP(dump1);
}

PRIVATE void condlinrec_(void)
{
    ONEPARAM("condlinrec");
    LIST("condlinrec");
    CHECKEMPTYLIST(stck->u.lis, "condlinrec");
    SAVESTACK;
    stck = SAVED2;
    condlinrecaux();
    POP(dump);
}

PRIVATE void condnestrecaux(void)
{
    int result = 0;
    dump1 = newnode(LIST_, SAVED1->u, dump1);
    dump2 = LIST_NEWNODE(stck, dump2);
    while (result == 0 && DMP1 != NULL && DMP1->next != NULL) {
        stck = DMP2;
        exeterm(DMP1->u.lis->u.lis);
        result = stck->u.num;
        if (!result)
            DMP1 = DMP1->next;
    }
    stck = DMP2;
    dump3 = LIST_NEWNODE((result ? DMP1->u.lis->next : DMP1->u.lis), dump3);
    exeterm(DMP3->u.lis);
    DMP3 = DMP3->next;
    while (DMP3 != NULL) {
        condnestrecaux();
        exeterm(DMP3->u.lis);
        DMP3 = DMP3->next;
    }
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

PRIVATE void condnestrec_(void)
{
    ONEPARAM("condnestrec");
    LIST("condnestrec");
    CHECKEMPTYLIST(stck->u.lis, "condnestrec");
    SAVESTACK;
    stck = SAVED2;
    condnestrecaux();
    POP(dump);
}

PRIVATE void linrecaux(void)
{
    int result;
    dump1 = LIST_NEWNODE(stck, dump1);
    exeterm(SAVED4->u.lis);
    result = stck->u.num;
    stck = DMP1;
    POP(dump1);
    if (result)
        exeterm(SAVED3->u.lis);
    else {
        exeterm(SAVED2->u.lis);
        linrecaux();
        exeterm(SAVED1->u.lis);
    }
}

PRIVATE void linrec_(void)
{
    FOURPARAMS("linrec");
    FOURQUOTES("linrec");
    SAVESTACK;
    stck = SAVED5;
    linrecaux();
    POP(dump);
}

PRIVATE void binrecaux(void)
{
    int result;
    dump1 = LIST_NEWNODE(stck, dump1);
    exeterm(SAVED4->u.lis);
    result = stck->u.num;
    stck = DMP1;
    POP(dump1);
    if (result)
        exeterm(SAVED3->u.lis);
    else {
        exeterm(SAVED2->u.lis); /* split */
        dump2 = newnode(stck->op, stck->u, dump2);
        POP(stck);
        binrecaux(); /* first */
        GNULLARY(dump2->op, dump2->u);
        POP(dump2);
        binrecaux(); /* second */
        exeterm(SAVED1->u.lis);
    } /* combine */
}

PRIVATE void binrec_(void)
{
    FOURPARAMS("binrec");
    FOURQUOTES("binrec");
    SAVESTACK;
    stck = SAVED5;
    binrecaux();
    POP(dump);
}

PRIVATE void treestepaux(Node* item)
{
    if (item->op != LIST_) {
        GNULLARY(item->op, item->u);
        exeterm(SAVED1->u.lis);
    } else {
        dump1 = newnode(LIST_, item->u, dump1);
        while (DMP1 != NULL) {
            treestepaux(DMP1);
            DMP1 = DMP1->next;
        }
        POP(dump1);
    }
}

PRIVATE void treestep_(void)
{
    TWOPARAMS("treestep");
    ONEQUOTE("treestep");
    SAVESTACK;
    stck = SAVED3;
    treestepaux(SAVED2);
    POP(dump);
}

PRIVATE void treerecaux(void)
{
    if (stck->next->op == LIST_) {
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(treerecaux, NULL));
        cons_(); /*  D  [[[O] C] ANON_FUNCT_]	*/
        D(printf("treerecaux: stack = ");)
        D(writeterm(stck, stdout); printf("\n");)
        exeterm(stck->u.lis->u.lis->next);
    } else {
        dump1 = newnode(LIST_, stck->u, dump1);
        POP(stck);
        exeterm(DMP1->u.lis);
        POP(dump1);
    }
}

PRIVATE void treerec_(void)
{
    THREEPARAMS("treerec");
    TWOQUOTES("treerec");
    cons_();
    D(printf("treerec: stack = "); writeterm(stck, stdout); printf("\n");)
    treerecaux();
}

PRIVATE void genrecaux(void)
{
    int result;
    D(printf("genrecaux: stack = ");)
    D(writeterm(stck, stdout); printf("\n");)
    SAVESTACK;
    POP(stck);
    exeterm(SAVED1->u.lis->u.lis); /*	[I]	*/
    result = stck->u.num;
    stck = SAVED2;
    if (result)
        exeterm(SAVED1->u.lis->next->u.lis); /*	[T]	*/
    else {
        exeterm(SAVED1->u.lis->next->next->u.lis); /*	[R1]	*/
        NULLARY(LIST_NEWNODE, SAVED1->u.lis);
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(genrecaux, NULL));
        cons_();
        exeterm(SAVED1->u.lis->next->next->next);
    } /*   [R2]	*/
    POP(dump);
}

PRIVATE void genrec_(void)
{
    FOURPARAMS("genrec");
    FOURQUOTES("genrec");
    cons_();
    cons_();
    cons_();
    genrecaux();
}

PRIVATE void treegenrecaux(void)
{
    D(printf("treegenrecaux: stack = ");)
    D(writeterm(stck, stdout); printf("\n");)
    if (stck->next->op == LIST_) {
        SAVESTACK; /* begin DIP	*/
        POP(stck);
        exeterm(SAVED1->u.lis->next->u.lis); /*	[O2]	*/
        GNULLARY(SAVED1->op, SAVED1->u);
        POP(dump); /*   end DIP	*/
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(treegenrecaux, NULL));
        cons_();
        exeterm(stck->u.lis->u.lis->next->next);
    } /*	[C]	*/
    else {
        dump1 = newnode(LIST_, stck->u, dump1);
        POP(stck);
        exeterm(DMP1->u.lis);
        POP(dump1);
    }
}

PRIVATE void treegenrec_(void)
{ /* T [O1] [O2] [C]	*/
    FOURPARAMS("treegenrec");
    THREEQUOTES("treegenrec");
    cons_();
    cons_();
    D(printf("treegenrec: stack = "); writeterm(stck, stdout); printf("\n");)
    treegenrecaux();
}

PRIVATE void plain_manual_(void) { make_manual(0); }

PRIVATE void html_manual_(void) { make_manual(1); }

PRIVATE void latex_manual_(void) { make_manual(2); }

#if 0
PRIVATE void manual_list_aux_(void)
{
    manual_list_(void);
}
#endif

/* - - - - -   I N I T I A L I S A T I O N   - - - - - */

static struct {
    char* name;
    void (*proc)(void);
    char *messg1, *messg2;
} optable[] = {
    /* THESE MUST BE DEFINED IN THE ORDER OF THEIR VALUES */
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

{"false",		dummy_,		"->  false",
"Pushes the value false."},

{"true",		dummy_,		"->  true",
"Pushes the value true."},

{"maxint",		dummy_,		"->  maxint",
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
"Pushes the integer value of current CPU usage in milliseconds."},

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

{"rollup",		rollup_,	"X Y Z  ->  Z X Y",
"Moves X and Y up, moves Z down"},

{"rolldown",		rolldown_,      "X Y Z  ->  Y Z X",
"Moves Y and Z down, moves X up"},

{"rotate",		rotate_,	"X Y Z  ->  Z Y X",
"Interchanges X and Z"},

{"popd",		popd_,		"Y Z  ->  Z",
"As if defined by:   popd  ==  [pop] dip "},

{"dupd",		dupd_,		"Y Z  ->  Y Y Z",
"As if defined by:   dupd  ==  [dup] dip"},

{"swapd",	       swapd_,		"X Y Z  ->  Y X Z",
"As if defined by:   swapd  ==  [swap] dip"},

{"rollupd",		rollupd_,       "X Y Z W  ->  Z X Y W",
"As if defined by:   rollupd  ==  [rollup] dip"},

{"rolldownd",		rolldownd_,     "X Y Z W  ->  Y Z X W",
"As if defined by:   rolldownd  ==  [rolldown] dip "},

{"rotated",		rotated_,       "X Y Z W  ->  Z Y X W",
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
"Converts a time I into a list T representing local time:\n[year month day hour minute second isdst yearday weekday].\nMonth is 1 = January ... 12 = December;\nisdst is a Boolean flagging daylight savings/summer time;\nweekday is 1 = Monday ... 7 = Sunday."},

{"gmtime",		gmtime_,	"I  ->  T",
"Converts a time I into a list T representing universal time:\n[year month day hour minute second isdst yearday weekday].\nMonth is 1 = January ... 12 = December;\nisdst is false; weekday is 1 = Monday ... 7 = Sunday."},

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

#ifdef FGET_FROM_FILE
{"fget",		fget_,		"S  ->  S F",
"Reads a factor from stream S and pushes it onto stack."},
#endif

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
"The file system object with pathname P is removed from the file system.\nB is a boolean indicating success or failure."},

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

{"fseek",		fseek_,		"S P W  ->  S B",
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
"I (=-1,0,+1) is the comparison of aggregates A and B.\nThe values correspond to the predicates <, =, >."},

{"at",			at_,		"A I  ->  X",
"X (= A[I]) is the member of A at position I."},

{"of",			of_,		"I A  ->  X",
"X (= A[I]) is the I-th member of aggregate A."},

{"size",		size_,		"A  ->  I",
"Integer I is the number of elements of aggregate A."},

{"opcase",		opcase_,	"X [..[X Xs]..]  ->  [Xs]",
"Indexing on type of X, returns the list [Xs]."},

{"case",		case_,		"X [..[X Y]..]  ->  [Y] i",
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

#ifdef SAMETYPE_BUILTIN
{"sametype",		sametype_,	"X Y  ->  B",
"Tests whether X and Y have the same type."},
#endif

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
"Executes P, pushes result R on stack."},

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

{"binrec",		binrec_,	"[P] [T] [R1] [R2]  ->  ...",
"Executes P. If that yields true, executes T.\nElse uses R1 to produce two intermediates, recurses on both,\nthen executes R2 to combines their results."},

{"genrec",		genrec_,	"[B] [T] [R1] [R2]  ->  ...",
"Executes B, if that yields true executes T.\nElse executes R1 and then [[[B] [T] [R1] R2] genrec] R2."},

{"condnestrec",		condnestrec_,	"[ [C1] [C2] .. [D] ]  ->  ...",
"A generalisation of condlinrec.\nEach [Ci] is of the form [[B] [R1] [R2] .. [Rn]] and [D] is of the form\n[[R1] [R2] .. [Rn]]. Tries each B, or if all fail, takes the default [D].\nFor the case taken, executes each [Ri] but recurses between any two\nconsecutive [Ri]. (n > 3 would be exceptional.)"},

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
"T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C."},

{"treegenrec",		treegenrec_,	"T [O1] [O2] [C]  ->  ...",
"T is a tree. If T is a leaf, executes O1.\nElse executes O2 and then [[[O1] [O2] C] treegenrec] C."},

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

{"__manual_list",	manual_list_,	"->  L",
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

#ifdef GETCH_AS_BUILTIN
{"getch",		getch_,		"->  F",
"Reads a character from input and pushes it onto stack."},
#endif

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

PUBLIC void inisymboltable(void) /* initialise		*/
{
    int i;
    symtabindex = symtab;
    for (i = 0; i < HASHSIZE; hashentry[i++] = symtab)
        ;
#if 0
    localentry = symtab;
#endif
    for (i = 0; optable[i].name; i++) {
        char* s = optable[i].name;
        HashValue(s);
        symtabindex->name = optable[i].name;
        symtabindex->u.proc = optable[i].proc;
        symtabindex->next = hashentry[hashvalue];
        hashentry[hashvalue] = symtabindex;
        D(printf("entered %s in symbol table at %p = %p\n", symtabindex->name,
              (void*)symtabindex, (void*)LOC2INT(symtabindex));)
        symtabindex++;
    }
    firstlibra = symtabindex;
}

PRIVATE void helpdetail_(void)
{
    Node* n;
    ONEPARAM("HELP");
    LIST("HELP");
    printf("\n");
    n = stck->u.lis;
    while (n != NULL) {
        if (n->op == USR_) {
            printf("%s  ==\n    ", n->u.ent->name);
            writeterm(n->u.ent->u.body, stdout);
            printf("\n");
            break;
        } else {
            Operator op;
            if ((op = n->op) == BOOLEAN_)
                op = n->u.num ? TRUE_ : FALSE_;
            if (op == INTEGER_ && n->u.num == MAXINT)
                op = MAXINT_;
            printf("%s	:   %s.\n%s\n", optable[(int)op].name,
                optable[(int)op].messg1, optable[(int)op].messg2);
        }
        printf("\n");
        n = n->next;
    }
    POP(stck);
}
#define PLAIN (style == 0)
#define HTML (style == 1)
#define LATEX (style == 2)
#define HEADER(N, NAME, HEAD)                                                  \
    if (strcmp(N, NAME) == 0) {                                                \
        printf("\n\n");                                                        \
        if (HTML)                                                              \
            printf("<DT><BR><B>");                                             \
        if (LATEX)                                                             \
            printf("\\item[--- \\BX{");                                        \
        printf("%s", HEAD);                                                    \
        if (LATEX)                                                             \
            printf("} ---] \\verb# #");                                        \
        if (HTML)                                                              \
            printf("</B><BR><BR>");                                            \
        printf("\n\n");                                                        \
    }

PRIVATE void make_manual(int style /* 0=plain, 1=HTML, 2=Latex */)
{
    int i;
    if (HTML)
        printf("<HTML>\n<DL>\n");
    for (i = BOOLEAN_; optable[i].name != 0; i++) {
        char* n = optable[i].name;
        HEADER(n, " truth value type", "literal") else
	HEADER(n, "false", "operand") else
	HEADER(n, "id", "operator") else
	HEADER(n, "null", "predicate") else
	HEADER(n, "i", "combinator") else
	HEADER(n, "help", "miscellaneous commands")
	if (n[0] != '_') {
            if (HTML)
                printf("\n<DT>");
            else if (LATEX) {
                if (n[0] == ' ') {
                    n++;
                    printf("\\item[\\BX{");
                } else
                    printf("\\item[\\JX{");
            }
            if (HTML && strcmp(n, "<=") == 0)
                printf("&lt;=");
            else
                printf("%s", n);
            if (LATEX)
                printf("}]  \\verb#");
            if (HTML)
                printf(" <CODE>      :  </CODE> ");
            /* the above line does not produce the spaces around ":" */
            else
                printf("\t:  ");
            printf("%s", optable[i].messg1);
            if (HTML)
                printf("\n<DD>");
            else if (LATEX)
                printf("# \\\\ \n {\\small\\verb#");
            else
                printf("\n");
            printf("%s", optable[i].messg2);
            if (LATEX)
                printf("#}");
            printf("\n\n");
        }
    }
    if (HTML)
        printf("\n</DL>\n</HTML>\n");
}

PRIVATE void manual_list_(void)
{
    int i = -1;
    while (optable[++i].name)
        ; /* find end */
    --i; /* overshot */
    dump1 = LIST_NEWNODE(0, dump1);
    dump2 = LIST_NEWNODE(0, dump2);
    while (i) {
        DMP1 = STRING_NEWNODE(optable[i].messg2, 0);
        DMP1 = STRING_NEWNODE(optable[i].messg1, DMP1);
        DMP1 = STRING_NEWNODE(optable[i].name, DMP1);
        DMP2 = LIST_NEWNODE(DMP1, DMP2);
        --i;
    }
    stck = LIST_NEWNODE(DMP2, stck);
    POP(dump2);
    POP(dump1);
}

PUBLIC char* opername(int o) { return optable[o].name; }
/* END of INTERP.C */
