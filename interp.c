/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.30.1.17
 *  date    : 04/12/21
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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "gc.h"
#include "globals.h"

void my_srand(unsigned num);
int my_rand(void);

PRIVATE void helpdetail_(pEnv env); /* this file */
PRIVATE void undefs_(pEnv env);
PRIVATE void make_manual(int style /* 0=plain, 1=html, 2=latex */);
PRIVATE void manual_list_(pEnv env);

#define ONEPARAM(NAME)                                                         \
    if (env->stck == NULL)                                                     \
    execerror(env, "one parameter", NAME)

#define TWOPARAMS(NAME)                                                        \
    if (env->stck == NULL || nextnode1(env->stck) == NULL)                     \
    execerror(env, "two parameters", NAME)

#define THREEPARAMS(NAME)                                                      \
    if (env->stck == NULL || nextnode1(env->stck) == NULL                      \
        || nextnode2(env->stck) == NULL)                                       \
    execerror(env, "three parameters", NAME)

#define FOURPARAMS(NAME)                                                       \
    if (env->stck == NULL || nextnode1(env->stck) == NULL                      \
        || nextnode2(env->stck) == NULL || nextnode3(env->stck) == NULL)       \
    execerror(env, "four parameters", NAME)

#define FIVEPARAMS(NAME)                                                       \
    if (env->stck == NULL || nextnode1(env->stck) == NULL                      \
        || nextnode2(env->stck) == NULL || nextnode3(env->stck) == NULL        \
        || nextnode4(env->stck) == NULL)                                       \
    execerror(env, "five parameters", NAME)

#define ONEQUOTE(NAME)                                                         \
    if (nodetype(env->stck) != LIST_)                                          \
    execerror(env, "quotation as top parameter", NAME)
#define TWOQUOTES(NAME)                                                        \
    ONEQUOTE(NAME);                                                            \
    if (nodetype(nextnode1(env->stck)) != LIST_)                               \
    execerror(env, "quotation as second parameter", NAME)
#define THREEQUOTES(NAME)                                                      \
    TWOQUOTES(NAME);                                                           \
    if (nodetype(nextnode2(env->stck)) != LIST_)                               \
    execerror(env, "quotation as third parameter", NAME)
#define FOURQUOTES(NAME)                                                       \
    THREEQUOTES(NAME);                                                         \
    if (nodetype(nextnode3(env->stck)) != LIST_)                               \
    execerror(env, "quotation as fourth parameter", NAME)
#define SAME2TYPES(NAME)                                                       \
    if (nodetype(env->stck) != nodetype(nextnode1(env->stck)))                 \
    execerror(env, "two parameters of the same type", NAME)
#define STRING(NAME)                                                           \
    if (nodetype(env->stck) != STRING_)                                        \
    execerror(env, "string", NAME)
#define STRING2(NAME)                                                          \
    if (nodetype(nextnode1(env->stck)) != STRING_)                             \
    execerror(env, "string as second parameter", NAME)
#define INTEGER(NAME)                                                          \
    if (nodetype(env->stck) != INTEGER_)                                       \
    execerror(env, "integer", NAME)
#define INTEGER2(NAME)                                                         \
    if (nodetype(nextnode1(env->stck)) != INTEGER_)                            \
    execerror(env, "integer as second parameter", NAME)
#define CHARACTER(NAME)                                                        \
    if (nodetype(env->stck) != CHAR_)                                          \
    execerror(env, "character", NAME)
#define INTEGERS2(NAME)                                                        \
    if (nodetype(env->stck) != INTEGER_                                        \
        || nodetype(nextnode1(env->stck)) != INTEGER_)                         \
    execerror(env, "two integers", NAME)
#define NUMERICTYPE(NAME)                                                      \
    if (nodetype(env->stck) != INTEGER_ && nodetype(env->stck) != CHAR_        \
        && nodetype(env->stck) != BOOLEAN_)                                    \
    execerror(env, "numeric", NAME)
#define NUMERIC2(NAME)                                                         \
    if (nodetype(nextnode1(env->stck)) != INTEGER_                             \
        && nodetype(nextnode1(env->stck)) != CHAR_)                            \
    execerror(env, "numeric second parameter", NAME)

#define FLOATABLE                                                              \
    (nodetype(env->stck) == INTEGER_ || nodetype(env->stck) == FLOAT_)

#define FLOATABLE2                                                             \
    ((nodetype(env->stck) == FLOAT_                                            \
         && nodetype(nextnode1(env->stck)) == FLOAT_)                          \
        || (nodetype(env->stck) == FLOAT_                                      \
               && nodetype(nextnode1(env->stck)) == INTEGER_)                  \
        || (nodetype(env->stck) == INTEGER_                                    \
               && nodetype(nextnode1(env->stck)) == FLOAT_))

#define FLOAT(NAME)                                                            \
    if (!FLOATABLE)                                                            \
        execerror(env, "float or integer", NAME);
#define FLOAT2(NAME)                                                           \
    if (!(FLOATABLE2 || (nodetype(env->stck) == INTEGER_                       \
                            && nodetype(nextnode1(env->stck)) == INTEGER_)))   \
    execerror(env, "two floats or integers", NAME)

#define FLOATVAL                                                               \
    (nodetype(env->stck) == FLOAT_ ? nodevalue(env->stck).dbl                  \
                                   : (double)nodevalue(env->stck).num)

#define FLOATVAL2                                                              \
    (nodetype(nextnode1(env->stck)) == FLOAT_                                  \
            ? nodevalue(nextnode1(env->stck)).dbl                              \
            : (double)nodevalue(nextnode1(env->stck)).num)

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

#define FILE(NAME)                                                             \
    if (nodetype(env->stck) != FILE_ || nodevalue(env->stck).fil == NULL)      \
    execerror(env, "file", NAME)
#define CHECKZERO(NAME)                                                        \
    if (nodevalue(env->stck).num == 0)                                         \
    execerror(env, "non-zero operand", NAME)
#define LIST(NAME)                                                             \
    if (nodetype(env->stck) != LIST_)                                          \
    execerror(env, "list", NAME)
#define LIST2(NAME)                                                            \
    if (nodetype(nextnode1(env->stck)) != LIST_)                               \
    execerror(env, "list as second parameter", NAME)
#define USERDEF(NAME)                                                          \
    if (nodetype(env->stck) != USR_)                                           \
    execerror(env, "user defined symbol", NAME)
#define CHECKLIST(OPR, NAME)                                                   \
    if (OPR != LIST_)                                                          \
    execerror(env, "internal list", NAME)
#define CHECKSETMEMBER(NODE, NAME)                                             \
    if ((nodetype(NODE) != INTEGER_ && nodetype(NODE) != CHAR_)                \
        || nodevalue(NODE).num >= SETSIZE)                                     \
    execerror(env, "small numeric", NAME)
#define CHECKEMPTYSET(SET, NAME)                                               \
    if (SET == 0)                                                              \
    execerror(env, "non-empty set", NAME)
#define CHECKEMPTYSTRING(STRING, NAME)                                         \
    if (*STRING == '\0')                                                       \
    execerror(env, "non-empty string", NAME)
#define CHECKEMPTYLIST(LIST, NAME)                                             \
    if (LIST == NULL)                                                          \
    execerror(env, "non-empty list", NAME)
#define INDEXTOOLARGE(NAME) execerror(env, "smaller index", NAME)
#define BADAGGREGATE(NAME) execerror(env, "aggregate parameter", NAME)
#define BADDATA(NAME) execerror(env, "different type", NAME)

#define DMP nodevalue(env->dump).lis
#define DMP1 nodevalue(env->dump1).lis
#define DMP2 nodevalue(env->dump2).lis
#define DMP3 nodevalue(env->dump3).lis
#define DMP4 nodevalue(env->dump4).lis
#define DMP5 nodevalue(env->dump5).lis
#define SAVESTACK env->dump = LIST_NEWNODE(env->stck, env->dump)
#define SAVED1 DMP
#define SAVED2 nextnode1(DMP)
#define SAVED3 nextnode2(DMP)
#define SAVED4 nextnode3(DMP)
#define SAVED5 nextnode4(DMP)
#define SAVED6 nextnode5(DMP)

#define POP(X) X = nextnode1(X)

#define NULLARY(CONSTRUCTOR, VALUE) env->stck = CONSTRUCTOR(VALUE, env->stck)
#define UNARY(CONSTRUCTOR, VALUE)                                              \
    env->stck = CONSTRUCTOR(VALUE, nextnode1(env->stck))
#define BINARY(CONSTRUCTOR, VALUE)                                             \
    env->stck = CONSTRUCTOR(VALUE, nextnode2(env->stck))
#define GNULLARY(TYPE, VALUE) env->stck = newnode(env, TYPE, (VALUE), env->stck)
#define GUNARY(TYPE, VALUE)                                                    \
    env->stck = newnode(env, TYPE, (VALUE), nextnode1(env->stck))
#define GBINARY(TYPE, VALUE)                                                   \
    env->stck = newnode(env, TYPE, (VALUE), nextnode2(env->stck))
#define GTERNARY(TYPE, VALUE)                                                  \
    env->stck = newnode(env, TYPE, (VALUE), nextnode3(env->stck))

#define GETSTRING(NODE)                                                        \
    (nodetype(NODE) == STRING_                                                 \
            ? nodevalue(NODE).str                                              \
            : (nodetype(NODE) == USR_ ? nodevalue(NODE).ent->name              \
                                      : opername(NODE->op)))

/* - - - -  O P E R A N D S   - - - - */

#define PUSH(PROCEDURE, CONSTRUCTOR, VALUE)                                    \
    PRIVATE void PROCEDURE(pEnv env) { NULLARY(CONSTRUCTOR, VALUE); }
#if 0
PUSH(true_, BOOLEAN_NEWNODE, 1L)                        /* constants        */
PUSH(false_, BOOLEAN_NEWNODE, 0L)
PUSH(maxint_, INTEGER_NEWNODE, (long_t)MAXINT)
#endif
PUSH(setsize_, INTEGER_NEWNODE, (long_t)SETSIZE)
PUSH(symtabmax_, INTEGER_NEWNODE, (long_t)SYMTABMAX)
PUSH(stdin_, FILE_NEWNODE, stdin)
PUSH(stdout_, FILE_NEWNODE, stdout)
PUSH(stderr_, FILE_NEWNODE, stderr)
PUSH(dump_, LIST_NEWNODE, env->dump) /* variables        */
PUSH(conts_, LIST_NEWNODE,
    LIST_NEWNODE(nextnode1(nodevalue(env->conts).lis), nextnode1(env->conts)))
PUSH(symtabindex_, INTEGER_NEWNODE, (long_t)symtabindex)
PUSH(rand_, INTEGER_NEWNODE, (long_t)my_rand())
#if 0
/* this is now in utils.c */
PUSH(memorymax_, INTEGER_NEWNODE, (long_t)MEMORYMAX)
PUSH(memoryindex_, INTEGER_NEWNODE, MEM2INT(memoryindex))
#endif
PUSH(echo_, INTEGER_NEWNODE, (long_t)echoflag)
PUSH(autoput_, INTEGER_NEWNODE, (long_t)autoput)
PUSH(undeferror_, INTEGER_NEWNODE, (long_t)undeferror)
PUSH(clock_, INTEGER_NEWNODE, (long_t)(clock() - startclock))
PUSH(time_, INTEGER_NEWNODE, (long_t)time(NULL))
PUSH(argc_, INTEGER_NEWNODE, (long_t)g_argc)

PUBLIC void stack_(pEnv env) { NULLARY(LIST_NEWNODE, env->stck); }

/* - - - - -   O P E R A T O R S   - - - - - */

PRIVATE void id_(pEnv env) { /* do nothing */}

PRIVATE void unstack_(pEnv env)
{
    ONEPARAM("unstack");
    LIST("unstack");
    env->stck = nodevalue(env->stck).lis;
}

#if 0
PRIVATE void newstack_(pEnv env)
{
    env->stck = NULL;
}
#endif

/* - - -   STACK   - - - */

PRIVATE void name_(pEnv env)
{
    ONEPARAM("name");
    UNARY(STRING_NEWNODE, nodetype(env->stck) == USR_
            ? vec_at(env->symtab, nodevalue(env->stck).ent).name
            : opername(nodetype(env->stck)));
}

PRIVATE void intern_(pEnv env)
{
    char *p;

    ONEPARAM("intern");
    STRING("intern");
    strncpy(ident, nodevalue(env->stck).str, ALEN);
    ident[ALEN - 1] = 0;
    p = 0;
    if (ident[0] == '-' || !strchr("(#)[]{}.;'\"0123456789", ident[0]))
        for (p = ident + 1; *p; p++)
            if (!isalnum((int)*p) && !strchr("=_-", *p))
                break;
    if (!p || *p)
        execerror(env, "valid name", ident);
    lookup(env, 0);
    if (location < firstlibra) {
        env->yylval.proc = vec_at(env->symtab, location).u.proc;
        GUNARY(location, env->yylval);
    } else
        UNARY(USR_NEWNODE, location);
}

PRIVATE void getenv_(pEnv env)
{
    char *str;

    ONEPARAM("getenv");
    STRING("getenv");
    if ((str = getenv(nodevalue(env->stck).str)) == 0)
        str = "";
    UNARY(STRING_NEWNODE, str);
}

PRIVATE void body_(pEnv env)
{
    ONEPARAM("body");
    USERDEF("body");
    UNARY(LIST_NEWNODE, vec_at(env->symtab, nodevalue(env->stck).ent).u.body);
}

PRIVATE void pop_(pEnv env)
{
    ONEPARAM("pop");
    POP(env->stck);
}

PRIVATE void swap_(pEnv env)
{
    TWOPARAMS("swap");
    SAVESTACK;
    GBINARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}

PRIVATE void rollup_(pEnv env)
{
    THREEPARAMS("rollup");
    SAVESTACK;
    GTERNARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}

PRIVATE void rolldown_(pEnv env)
{
    THREEPARAMS("rolldown");
    SAVESTACK;
    GTERNARY(nodetype(SAVED2), nodevalue(SAVED2));
    GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    POP(env->dump);
}

PRIVATE void rotate_(pEnv env)
{
    THREEPARAMS("rotate");
    SAVESTACK;
    GTERNARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    POP(env->dump);
}

PRIVATE void dup_(pEnv env)
{
    ONEPARAM("dup");
    GNULLARY(nodetype(env->stck), nodevalue(env->stck));
}

#define DIPPED(PROCEDURE, NAME, PARAMCOUNT, ARGUMENT)                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        PARAMCOUNT(NAME);                                                      \
        SAVESTACK;                                                             \
        POP(env->stck);                                                        \
        ARGUMENT(env);                                                         \
        GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));                         \
        POP(env->dump);                                                        \
    }

DIPPED(popd_, "popd", TWOPARAMS, pop_)
DIPPED(dupd_, "dupd", TWOPARAMS, dup_)
DIPPED(swapd_, "swapd", THREEPARAMS, swap_)
DIPPED(rolldownd_, "rolldownd", FOURPARAMS, rolldown_)
DIPPED(rollupd_, "rollupd", FOURPARAMS, rollup_)
DIPPED(rotated_, "rotated", FOURPARAMS, rotate_)

/* - - -   BOOLEAN   - - - */

#define ANDORXOR(PROCEDURE, NAME, OPER1, OPER2)                                \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        SAME2TYPES(NAME);                                                      \
        switch (nodetype(env->stck)) {                                         \
        case SET_:                                                             \
            BINARY(SET_NEWNODE, (long_t)(nodevalue(nextnode1(env->stck))       \
                                             .set OPER1 nodevalue(env->stck)   \
                                             .set));                           \
            return;                                                            \
        case BOOLEAN_:                                                         \
        case CHAR_:                                                            \
        case INTEGER_:                                                         \
            BINARY(                                                            \
                BOOLEAN_NEWNODE, (long_t)(nodevalue(nextnode1(env->stck))      \
                                              .num OPER2 nodevalue(env->stck)  \
                                              .num));                          \
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
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        NUMERICTYPE(NAME);                                                     \
        UNARY(RESULTTYP, nodevalue(env->stck).num);                            \
    }

ORDCHR(ord_, "ord", INTEGER_NEWNODE)
ORDCHR(chr_, "chr", CHAR_NEWNODE)

PRIVATE void abs_(pEnv env)
{
    ONEPARAM("abs");
    /* start new */
    FLOAT("abs");
    if (nodetype(env->stck) == INTEGER_) {
        if (nodevalue(env->stck).num < 0)
            UNARY(INTEGER_NEWNODE, -nodevalue(env->stck).num);
        return;
    }
    /* end new */
    FLOAT_U(fabs);
}

PRIVATE double fsgn(double f)
{
    if (f < 0)
        return -1.0;
    if (f > 0)
        return 1.0;
    return 0.0;
}

PRIVATE void sign_(pEnv env)
{
    ONEPARAM("sign");
    /* start new */
    FLOAT("sign");
    if (nodetype(env->stck) == INTEGER_) {
        if (nodevalue(env->stck).num != 0 && nodevalue(env->stck).num != 1)
            UNARY(INTEGER_NEWNODE, nodevalue(env->stck).num > 0 ? 1 : -1);
        return;
    }
    /* end new */
    FLOAT_U(fsgn);
}

PRIVATE void neg_(pEnv env)
{
    ONEPARAM("neg");
    /* start new */
    FLOAT("neg");
    if (nodetype(env->stck) == INTEGER_) {
        if (nodevalue(env->stck).num)
            UNARY(INTEGER_NEWNODE, -nodevalue(env->stck).num);
        return;
    }
    /* end new */
    FLOAT_U(-);
}

PRIVATE void mul_(pEnv env)
{
    TWOPARAMS("*");
    FLOAT_I(*);
    INTEGERS2("*");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num * nodevalue(env->stck).num);
}

PRIVATE void divide_(pEnv env)
{
    TWOPARAMS("/");
    if ((nodetype(env->stck) == FLOAT_ && nodevalue(env->stck).dbl == 0.0)
        || (nodetype(env->stck) == INTEGER_ && nodevalue(env->stck).num == 0))
        execerror(env, "non-zero divisor", "/");
    FLOAT_I(/);
    INTEGERS2("/");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num / nodevalue(env->stck).num);
}

PRIVATE void rem_(pEnv env)
{
    TWOPARAMS("rem");
    FLOAT_P(fmod);
    INTEGERS2("rem");
    CHECKZERO("rem");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num % nodevalue(env->stck).num);
}

PRIVATE void div_(pEnv env)
{
    ldiv_t result; /* BIT_32 */

    TWOPARAMS("div");
    INTEGERS2("div");
    CHECKZERO("div");
    result
        = ldiv(nodevalue(nextnode1(env->stck)).num, nodevalue(env->stck).num);
    BINARY(INTEGER_NEWNODE, result.quot);
    NULLARY(INTEGER_NEWNODE, result.rem);
}

PRIVATE void strtol_(pEnv env) /* BIT_32 */
{
    TWOPARAMS("strtol");
    SAVESTACK;
    INTEGER("strtol");
    POP(env->stck);
    STRING("strtol");
    UNARY(INTEGER_NEWNODE,
        strtol(nodevalue(SAVED2).str, NULL, nodevalue(SAVED1).num));
    POP(env->dump);
}

PRIVATE void strtod_(pEnv env)
{
    ONEPARAM("strtod");
    STRING("strtod");
    UNARY(FLOAT_NEWNODE, strtod(nodevalue(env->stck).str, NULL));
}

PRIVATE void format_(pEnv env)
{
    int width, prec;
    char spec, format[7], *result;
#ifdef USE_SNPRINTF
    int leng;
#endif
    FOURPARAMS("format");
    INTEGER("format");
    INTEGER2("format");
    prec = nodevalue(env->stck).num;
    POP(env->stck);
    width = nodevalue(env->stck).num;
    POP(env->stck);
    CHARACTER("format");
    spec = (char)nodevalue(env->stck).num;
    POP(env->stck);
    if (!strchr("dioxX", spec))
        execerror(env, "one of: d i o x X", "format");
    strcpy(format, "%*.*ld");
    format[5] = spec;
    NUMERICTYPE("format");
#ifdef USE_SNPRINTF
    leng = snprintf(0, 0, format, width, prec, nodevalue(env->stck).num) + 1;
    result = GC_malloc_atomic(leng + 1);
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).num);
#else
    result = GC_malloc_atomic(INPLINEMAX); /* should be sufficient */
    sprintf(result, format, width, prec, env->stck->u.num);
#endif
    UNARY(STRING_NEWNODE, result);
}

PRIVATE void formatf_(pEnv env)
{
    int width, prec;
    char spec, format[7], *result;
#ifdef USE_SNPRINTF
    int leng;
#endif
    FOURPARAMS("formatf");
    INTEGER("formatf");
    INTEGER2("formatf");
    prec = nodevalue(env->stck).num;
    POP(env->stck);
    width = nodevalue(env->stck).num;
    POP(env->stck);
    CHARACTER("formatf");
    spec = (char)nodevalue(env->stck).num;
    POP(env->stck);
    if (!strchr("eEfgG", spec))
        execerror(env, "one of: e E f g G", "formatf");
    strcpy(format, "%*.*lg");
    format[5] = spec;
    FLOAT("formatf");
#ifdef USE_SNPRINTF
    leng = snprintf(0, 0, format, width, prec, nodevalue(env->stck).num) + 1;
    result = GC_malloc_atomic(leng + 1);
    snprintf(result, leng, format, width, prec, nodevalue(env->stck).dbl);
#else
    result = GC_malloc_atomic(INPLINEMAX); /* should be sufficient */
    sprintf(result, format, width, prec, nodevalue(env->stck).dbl);
#endif
    UNARY(STRING_NEWNODE, result);
}

/* - - -   TIME   - - - */

#ifndef USE_TIME_REC
void localtime_r(time_t *t, struct tm *tm) { *tm = *localtime(t); }

void gmtime_r(time_t *t, struct tm *tm) { *tm = *gmtime(t); }
#endif

#define UNMKTIME(PROCEDURE, NAME, FUNC)                                        \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        struct tm t;                                                           \
        long_t wday;                                                           \
        time_t timval;                                                         \
        ONEPARAM(NAME);                                                        \
        INTEGER(NAME);                                                         \
        timval = nodevalue(env->stck).num;                                     \
        FUNC(&timval, &t);                                                     \
        wday = t.tm_wday;                                                      \
        if (wday == 0)                                                         \
            wday = 7;                                                          \
        env->dump1 = LIST_NEWNODE(NULL, env->dump1);                           \
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
        POP(env->dump1);                                                       \
    }

UNMKTIME(localtime_, "localtime", localtime_r)
UNMKTIME(gmtime_, "gmtime", gmtime_r)

PRIVATE void decode_time(pEnv env, struct tm *t)
{
    Index p;
    t->tm_year = t->tm_mon = t->tm_mday = t->tm_hour = t->tm_min = t->tm_sec
        = t->tm_isdst = t->tm_yday = t->tm_wday = 0;
    p = nodevalue(env->stck).lis;
    if (p && nodetype(p) == INTEGER_) {
        t->tm_year = nodevalue(p).num - 1900;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_mon = nodevalue(p).num - 1;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_mday = nodevalue(p).num;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_hour = nodevalue(p).num;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_min = nodevalue(p).num;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_sec = nodevalue(p).num;
        POP(p);
    }
    if (p && nodetype(p) == BOOLEAN_) {
        t->tm_isdst = nodevalue(p).num;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_yday = nodevalue(p).num;
        POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
        t->tm_wday = nodevalue(p).num;
        POP(p);
    }
}

PRIVATE void mktime_(pEnv env)
{
    struct tm t;

    ONEPARAM("mktime");
    LIST("mktime");
    decode_time(env, &t);
    UNARY(INTEGER_NEWNODE, (long_t)mktime(&t));
}

PRIVATE void strftime_(pEnv env)
{
    struct tm t;
    char *fmt, *result;
    size_t length;

    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = nodevalue(env->stck).str;
    POP(env->stck);
    LIST("strftime");
    decode_time(env, &t);
    length = INPLINEMAX;
    result = GC_malloc_atomic(length);
    strftime(result, length, fmt, &t);
    UNARY(STRING_NEWNODE, result);
}

/* - - -   FLOAT   - - - */

#define UFLOAT(PROCEDURE, NAME, FUNC)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
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
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        FLOAT2(NAME);                                                          \
        BINARY(FLOAT_NEWNODE, FUNC(FLOATVAL2, FLOATVAL));                      \
    }
BFLOAT(atan2_, "atan2", atan2)
BFLOAT(pow_, "pow", pow)

PRIVATE void frexp_(pEnv env)
{
    int exp;

    ONEPARAM("frexp");
    FLOAT("frexp");
    UNARY(FLOAT_NEWNODE, frexp(FLOATVAL, &exp));
    NULLARY(INTEGER_NEWNODE, (long_t)exp);
}

PRIVATE void modf_(pEnv env)
{
    double exp;

    ONEPARAM("modf");
    FLOAT("modf");
    UNARY(FLOAT_NEWNODE, modf(FLOATVAL, &exp));
    NULLARY(FLOAT_NEWNODE, exp);
}

PRIVATE void ldexp_(pEnv env)
{
    int exp;

    TWOPARAMS("ldexp");
    INTEGER("ldexp");
    exp = nodevalue(env->stck).num;
    POP(env->stck);
    FLOAT("ldexp");
    UNARY(FLOAT_NEWNODE, ldexp(FLOATVAL, exp));
}

PRIVATE void trunc_(pEnv env)
{
    ONEPARAM("trunc");
    FLOAT("trunc");
    UNARY(INTEGER_NEWNODE, (long_t)FLOATVAL);
}

/* - - -   NUMERIC   - - - */

#define PREDSUCC(PROCEDURE, NAME, OPER)                                        \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        NUMERICTYPE(NAME);                                                     \
        if (nodetype(env->stck) == CHAR_)                                      \
            UNARY(CHAR_NEWNODE, nodevalue(env->stck).num OPER 1);              \
        else                                                                   \
            UNARY(INTEGER_NEWNODE, nodevalue(env->stck).num OPER 1);           \
    }

PREDSUCC(pred_, "pred", -)
PREDSUCC(succ_, "succ", +)

#define PLUSMINUS(PROCEDURE, NAME, OPER)                                       \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        FLOAT_I(OPER);                                                         \
        INTEGER(NAME);                                                         \
        NUMERIC2(NAME);                                                        \
        if (nodetype(nextnode1(env->stck)) == CHAR_)                           \
            BINARY(CHAR_NEWNODE, nodevalue(nextnode1(env->stck))               \
                                     .num OPER nodevalue(env->stck)            \
                                     .num);                                    \
        else                                                                   \
            BINARY(INTEGER_NEWNODE, nodevalue(nextnode1(env->stck))            \
                                        .num OPER nodevalue(env->stck)         \
                                        .num);                                 \
    }

PLUSMINUS(plus_, "+", +)
PLUSMINUS(minus_, "-", -)

#define MAXMIN(PROCEDURE, NAME, OPER)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        if (FLOATABLE2) {                                                      \
            BINARY(FLOAT_NEWNODE,                                              \
                FLOATVAL OPER FLOATVAL2 ? FLOATVAL2 : FLOATVAL);               \
            return;                                                            \
        }                                                                      \
        SAME2TYPES(NAME);                                                      \
        NUMERICTYPE(NAME);                                                     \
        if (nodetype(env->stck) == CHAR_)                                      \
            BINARY(CHAR_NEWNODE, nodevalue(env->stck)                          \
                                     .num OPER nodevalue(nextnode1(env->stck)) \
                                     .num                                      \
                    ? nodevalue(nextnode1(env->stck)).num                      \
                    : nodevalue(env->stck).num);                               \
        else                                                                   \
            BINARY(                                                            \
                INTEGER_NEWNODE, nodevalue(env->stck)                          \
                                     .num OPER nodevalue(nextnode1(env->stck)) \
                                     .num                                      \
                    ? nodevalue(nextnode1(env->stck)).num                      \
                    : nodevalue(env->stck).num);                               \
    }

MAXMIN(max_, "max", <)
MAXMIN(min_, "min", >)

PRIVATE double Compare(pEnv env, Index first, Index second, int *error)
{
    *error = 0;
    switch (nodetype(first)) {
    case USR_:
        switch (nodetype(second)) {
        case USR_:
            return strcmp(vec_at(env->symtab, nodevalue(first).ent).name,
                vec_at(env->symtab, nodevalue(second).ent).name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(vec_at(env->symtab, nodevalue(first).ent).name,
                nodevalue(second).str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(vec_at(env->symtab, nodevalue(first).ent).name,
                opername(nodetype(second)));
        }
        break;
    case ANON_FUNCT_:
        switch (nodetype(second)) {
        case USR_:
            break;
        case ANON_FUNCT_:
            return (size_t)nodevalue(first).proc
                - (size_t)nodevalue(second).proc;
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
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).num - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).num - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case CHAR_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).num - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).num - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case INTEGER_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).num - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).num - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case SET_:
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            break;
        case SET_:
            return nodevalue(first).set - nodevalue(second).set;
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
        default:
            break;
        }
        break;
    case STRING_:
        switch (nodetype(second)) {
        case USR_:
            return strcmp(nodevalue(first).str,
                vec_at(env->symtab, nodevalue(second).ent).name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(nodevalue(first).str, nodevalue(second).str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(nodevalue(first).str, opername(nodetype(second)));
        }
        break;
    case LIST_:
        switch (nodetype(second)) {
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
        switch (nodetype(second)) {
        case USR_:
        case ANON_FUNCT_:
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
            return nodevalue(first).dbl - nodevalue(second).num;
        case SET_:
        case STRING_:
        case LIST_:
            break;
        case FLOAT_:
            return nodevalue(first).dbl - nodevalue(second).dbl;
        case FILE_:
        default:
            break;
        }
        break;
    case FILE_:
        switch (nodetype(second)) {
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
            return nodevalue(first).fil - nodevalue(second).fil;
        default:
            break;
        }
        break;
    default:
        switch (nodetype(second)) {
        case USR_:
            return strcmp(opername(nodetype(first)),
                vec_at(env->symtab, nodevalue(second).ent).name);
        case ANON_FUNCT_:
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
            break;
        case STRING_:
            return strcmp(opername(nodetype(first)), nodevalue(second).str);
        case LIST_:
        case FLOAT_:
        case FILE_:
            break;
        default:
            return strcmp(
                opername(nodetype(first)), opername(nodetype(second)));
        }
        break;
    }
    *error = 1;
    return 0;
}

#define COMPREL(PROCEDURE, NAME, CONSTRUCTOR, OPR, SETCMP)                     \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        double cmp;                                                            \
        int comp = 0, error, i, j;                                             \
        TWOPARAMS(NAME);                                                       \
        if (nodetype(env->stck) == SET_) {                                     \
            i = nodevalue(nextnode1(env->stck)).num;                           \
            j = nodevalue(env->stck).num;                                      \
            comp = SETCMP;                                                     \
        } else {                                                               \
            cmp = Compare(env, nextnode1(env->stck), env->stck, &error);       \
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
        env->stck = CONSTRUCTOR(comp, nextnode2(env->stck));                   \
    }

COMPREL(eql_, "=", BOOLEAN_NEWNODE, ==, i == j)
COMPREL(neql_, "!=", BOOLEAN_NEWNODE, !=, i != j)
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))
COMPREL(leql_, "<=", BOOLEAN_NEWNODE, <=, !(i & ~j))
COMPREL(greater_, ">", BOOLEAN_NEWNODE, >, i != j && !(j & ~i))
COMPREL(geql_, ">=", BOOLEAN_NEWNODE, >=, !(j & ~i))
COMPREL(compare_, "compare", INTEGER_NEWNODE, +, i - j < 0 ? -1 : i - j > 0)

#ifdef SAMETYPE_BUILTIN
PRIVATE void sametype_(pEnv env)
{
    TWOPARAMS("sametype");
    BINARY(
        BOOLEAN_NEWNODE, nodetype(env->stck) == nodetype(nextnode1(env->stck)));
}
#endif

/* - - -   FILES AND STREAMS   - - - */

PRIVATE void fopen_(pEnv env)
{
    TWOPARAMS("fopen");
    STRING("fopen");
    STRING2("fopen");
    BINARY(FILE_NEWNODE,
        fopen(nodevalue(nextnode1(env->stck)).str, nodevalue(env->stck).str));
}

PRIVATE void fclose_(pEnv env)
{
    ONEPARAM("fclose");
    if (nodetype(env->stck) == FILE_ && nodevalue(env->stck).fil == NULL) {
        POP(env->stck);
        return;
    }
    FILE("fclose");
    fclose(nodevalue(env->stck).fil);
    POP(env->stck);
}

PRIVATE void fflush_(pEnv env)
{
    ONEPARAM("fflush");
    FILE("fflush");
    fflush(nodevalue(env->stck).fil);
}

PRIVATE void fremove_(pEnv env)
{
    ONEPARAM("fremove");
    STRING("fremove");
    UNARY(BOOLEAN_NEWNODE, (long_t)!remove(nodevalue(env->stck).str));
}

PRIVATE void frename_(pEnv env)
{
    TWOPARAMS("frename");
    STRING("frename");
    STRING2("frename");
    BINARY(BOOLEAN_NEWNODE, (long_t)!rename(nodevalue(nextnode1(env->stck)).str,
                                nodevalue(env->stck).str));
}

#define FILEGET(PROCEDURE, NAME, CONSTRUCTOR, EXPR)                            \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        FILE(NAME);                                                            \
        NULLARY(CONSTRUCTOR, EXPR);                                            \
    }

FILEGET(feof_, "feof", BOOLEAN_NEWNODE, (long_t)feof(nodevalue(env->stck).fil))
FILEGET(ferror_, "ferror", BOOLEAN_NEWNODE,
    (long_t)ferror(nodevalue(env->stck).fil))
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, (long_t)getc(nodevalue(env->stck).fil))
FILEGET(ftell_, "ftell", INTEGER_NEWNODE, ftell(nodevalue(env->stck).fil))

#ifdef GETCH_AS_BUILTIN
PRIVATE void getch_(pEnv env) { NULLARY(CHAR_NEWNODE, (long_t)getchar()); }
#endif

PRIVATE void fgets_(pEnv env)
{
    char *buf;
    size_t leng = 0, size = INPLINEMAX;

    ONEPARAM("fgets");
    FILE("fgets");
    buf = GC_malloc_atomic(size);
    buf[leng = 0] = 0;
    while (fgets(buf + leng, size - leng, nodevalue(env->stck).fil)) {
        if ((leng = strlen(buf)) > 0 && buf[leng - 1] == '\n')
            break;
        buf = GC_realloc(buf, size <<= 1);
    }
    NULLARY(STRING_NEWNODE, buf);
}

PRIVATE void fput_(pEnv env)
{
    FILE *stm = NULL;

    TWOPARAMS("fput");
    if (nodetype(nextnode1(env->stck)) != FILE_
        || (stm = nodevalue(nextnode1(env->stck)).fil) == NULL)
        execerror(env, "file", "fput");
    stm = nodevalue(nextnode1(env->stck)).fil;
    writefactor(env, env->stck, stm);
    fprintf(stm, " ");
    POP(env->stck);
}

#ifdef FGET_FROM_FILE
PRIVATE void fget_(pEnv env)
{
    FILE *stm = NULL;

    ONEPARAM("fget");
    if (nodetype(env->stck) != FILE_
        || (stm = nodevalue(env->stck).fil) == NULL)
        execerror(env, "file", "fget");
    stm = nodevalue(env->stck).fil;
    redirect(env, stm);
    getsym(env);
    readfactor(env, 0);
}
#endif

PRIVATE void fputch_(pEnv env)
{
    int ch;

    TWOPARAMS("fputch");
    INTEGER("fputch");
    ch = nodevalue(env->stck).num;
    POP(env->stck);
    FILE("fputch");
    putc(ch, nodevalue(env->stck).fil);
}

PRIVATE void fputchars_(
    pEnv env) /* suggested by Heiko Kuhrt, as "fputstring_" */
{
    FILE *stm = NULL;

    TWOPARAMS("fputchars");
    if (nodetype(nextnode1(env->stck)) != FILE_
        || (stm = nodevalue(nextnode1(env->stck)).fil) == NULL)
        execerror(env, "file", "fputchars");
    stm = nodevalue(nextnode1(env->stck)).fil;
    fprintf(stm, "%s", nodevalue(env->stck).str);
    POP(env->stck);
}

PRIVATE void fread_(pEnv env)
{
    unsigned char *buf;
    long_t count;

    TWOPARAMS("fread");
    INTEGER("fread");
    count = nodevalue(env->stck).num;
    POP(env->stck);
    FILE("fread");
    buf = GC_malloc_atomic(count);
    env->dump1 = LIST_NEWNODE(NULL, env->dump1);
    for (count
         = fread(buf, (size_t)1, (size_t)count, nodevalue(env->stck).fil) - 1;
         count >= 0; count--)
        DMP1 = INTEGER_NEWNODE((long_t)buf[count], DMP1);
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
}

PRIVATE void fwrite_(pEnv env)
{
    int length, i;
    unsigned char *buff;
    Index n;

    TWOPARAMS("fwrite");
    LIST("fwrite");
    for (n = nodevalue(env->stck).lis, length = 0; n;
         n = nextnode1(n), length++)
        if (nodetype(n) != INTEGER_)
            execerror(env, "numeric list", "fwrite");
    buff = GC_malloc_atomic(length);
    for (n = nodevalue(env->stck).lis, i = 0; n; n = nextnode1(n), i++)
        buff[i] = (char)nodevalue(n).num;
    POP(env->stck);
    FILE("fwrite");
    fwrite(buff, (size_t)length, (size_t)1, nodevalue(env->stck).fil);
}

PRIVATE void fseek_(pEnv env)
{
    long pos;
    int whence;

    THREEPARAMS("fseek");
    INTEGER("fseek");
    INTEGER2("fseek");
    whence = nodevalue(env->stck).num;
    POP(env->stck);
    pos = nodevalue(env->stck).num;
    POP(env->stck);
    FILE("fseek");
    NULLARY(BOOLEAN_NEWNODE,
        (long_t) !!fseek(nodevalue(env->stck).fil, pos, whence));
}

/* - - -   AGGREGATES   - - - */

PRIVATE void first_(pEnv env)
{
    ONEPARAM("first");
    switch (nodetype(env->stck)) {
    case LIST_:
        CHECKEMPTYLIST(nodevalue(env->stck).lis, "first");
        GUNARY(nodetype(nodevalue(env->stck).lis),
            nodevalue(nodevalue(env->stck).lis));
        return;
    case STRING_:
        CHECKEMPTYSTRING(nodevalue(env->stck).str, "first");
        UNARY(CHAR_NEWNODE, (long_t) * (nodevalue(env->stck).str));
        return;
    case SET_: {
        int i = 0;
        CHECKEMPTYSET(nodevalue(env->stck).set, "first");
        while (!(nodevalue(env->stck).set & ((long_t)1 << i)))
            i++;
        UNARY(INTEGER_NEWNODE, i);
        return;
    }
    default:
        BADAGGREGATE("first");
    }
}

PRIVATE void rest_(pEnv env)
{
    ONEPARAM("rest");
    switch (nodetype(env->stck)) {
    case SET_: {
        int i = 0;
        CHECKEMPTYSET(nodevalue(env->stck).set, "rest");
        while (!(nodevalue(env->stck).set & ((long_t)1 << i)))
            i++;
        UNARY(SET_NEWNODE, nodevalue(env->stck).set & ~((long_t)1 << i));
        break;
    }
    case STRING_: {
        char *s = nodevalue(env->stck).str;
        CHECKEMPTYSTRING(s, "rest");
        UNARY(STRING_NEWNODE, GC_strdup(++s));
        break;
    }
    case LIST_:
        CHECKEMPTYLIST(nodevalue(env->stck).lis, "rest");
        UNARY(LIST_NEWNODE, nextnode1(nodevalue(env->stck).lis));
        return;
    default:
        BADAGGREGATE("rest");
    }
}

PRIVATE void uncons_(pEnv env)
{
    ONEPARAM("uncons");
    switch (nodetype(env->stck)) {
    case SET_: {
        int i = 0;
        long_t set = nodevalue(env->stck).set;
        CHECKEMPTYSET(set, "uncons");
        while (!(set & ((long_t)1 << i)))
            i++;
        UNARY(INTEGER_NEWNODE, i);
        NULLARY(SET_NEWNODE, set & ~((long_t)1 << i));
        break;
    }
    case STRING_: {
        char *s = nodevalue(env->stck).str;
        CHECKEMPTYSTRING(s, "uncons");
        UNARY(CHAR_NEWNODE, (long_t)*s);
        NULLARY(STRING_NEWNODE, GC_strdup(++s));
        break;
    }
    case LIST_:
        SAVESTACK;
        CHECKEMPTYLIST(nodevalue(SAVED1).lis, "uncons");
        GUNARY(
            nodetype(nodevalue(SAVED1).lis), nodevalue(nodevalue(SAVED1).lis));
        NULLARY(LIST_NEWNODE, nextnode1(nodevalue(SAVED1).lis));
        POP(env->dump);
        return;
    default:
        BADAGGREGATE("uncons");
    }
}

PRIVATE void unswons_(pEnv env)
{
    ONEPARAM("unswons");
    switch (nodetype(env->stck)) {
    case SET_: {
        int i = 0;
        long_t set = nodevalue(env->stck).set;
        CHECKEMPTYSET(set, "unswons");
        while (!(set & ((long_t)1 << i)))
            i++;
        UNARY(SET_NEWNODE, set & ~((long_t)1 << i));
        NULLARY(INTEGER_NEWNODE, i);
        break;
    }
    case STRING_: {
        char *s = nodevalue(env->stck).str;
        CHECKEMPTYSTRING(s, "unswons");
        UNARY(STRING_NEWNODE, GC_strdup(++s));
        NULLARY(CHAR_NEWNODE, (long_t) * (--s));
        break;
    }
    case LIST_:
        SAVESTACK;
        CHECKEMPTYLIST(nodevalue(SAVED1).lis, "unswons");
        UNARY(LIST_NEWNODE, nextnode1(nodevalue(SAVED1).lis));
        GNULLARY(
            nodetype(nodevalue(SAVED1).lis), nodevalue(nodevalue(SAVED1).lis));
        POP(env->dump);
        return;
    default:
        BADAGGREGATE("unswons");
    }
}

PRIVATE int equal_aux(pEnv env, Index n1, Index n2); /* forward */

PRIVATE int equal_list_aux(pEnv env, Index n1, Index n2)
{
    if (n1 == NULL && n2 == NULL)
        return 1;
    if (n1 == NULL || n2 == NULL)
        return 0;
    if (equal_aux(env, n1, n2))
        return equal_list_aux(env, nextnode1(n1), nextnode1(n2));
    else
        return 0;
}

PRIVATE int equal_aux(pEnv env, Index n1, Index n2)
{
    int error;

    if (n1 == NULL && n2 == NULL)
        return 1;
    if (n1 == NULL || n2 == NULL)
        return 0;
    if (nodetype(n1) == LIST_ && nodetype(n2) == LIST_)
        return equal_list_aux(env, nodevalue(n1).lis, nodevalue(n2).lis);
    return !Compare(env, n1, n2, &error) && !error;
}

PRIVATE void equal_(pEnv env)
{
    TWOPARAMS("equal");
    BINARY(BOOLEAN_NEWNODE, equal_aux(env, env->stck, nextnode1(env->stck)));
}

#define INHAS(PROCEDURE, NAME, AGGR, ELEM)                                     \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        int found = 0, error;                                                  \
        TWOPARAMS(NAME);                                                       \
        switch (nodetype(AGGR)) {                                              \
        case SET_:                                                             \
            found                                                              \
                = ((nodevalue(AGGR).set) & ((long_t)1 << nodevalue(ELEM).num)) \
                > 0;                                                           \
            break;                                                             \
        case STRING_: {                                                        \
            char *s;                                                           \
            for (s = nodevalue(AGGR).str;                                      \
                 s && *s != '\0' && *s != nodevalue(ELEM).num; s++)            \
                ;                                                              \
            found = s && *s != '\0';                                           \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            Index n = nodevalue(AGGR).lis;                                     \
            while (n != NULL && (Compare(env, n, ELEM, &error) || error))      \
                n = nextnode1(n);                                              \
            found = n != NULL;                                                 \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        BINARY(BOOLEAN_NEWNODE, (long_t)found);                                \
    }

INHAS(in_, "in", env->stck, nextnode1(env->stck))
INHAS(has_, "has", nextnode1(env->stck), env->stck)

#define OF_AT(PROCEDURE, NAME, AGGR, INDEX)                                    \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        if (nodetype(INDEX) != INTEGER_ || nodevalue(INDEX).num < 0)           \
            execerror(env, "non-negative integer", NAME);                      \
        switch (nodetype(AGGR)) {                                              \
        case SET_: {                                                           \
            int i, indx = nodevalue(INDEX).num;                                \
            CHECKEMPTYSET(nodevalue(AGGR).set, NAME);                          \
            for (i = 0; i < SETSIZE; i++) {                                    \
                if (nodevalue(AGGR).set & ((long_t)1 << i)) {                  \
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
            if (strlen(nodevalue(AGGR).str) < (size_t)nodevalue(INDEX).num)    \
                INDEXTOOLARGE(NAME);                                           \
            BINARY(CHAR_NEWNODE,                                               \
                (long_t)nodevalue(AGGR).str[nodevalue(INDEX).num]);            \
            return;                                                            \
        case LIST_: {                                                          \
            Index n = nodevalue(AGGR).lis;                                     \
            int i = nodevalue(INDEX).num;                                      \
            CHECKEMPTYLIST(n, NAME);                                           \
            while (i > 0) {                                                    \
                if (nextnode1(n) == NULL)                                      \
                    INDEXTOOLARGE(NAME);                                       \
                n = nextnode1(n);                                              \
                i--;                                                           \
            }                                                                  \
            GBINARY(nodetype(n), nodevalue(n));                                \
            return;                                                            \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }

OF_AT(of_, "of", env->stck, nextnode1(env->stck))
OF_AT(at_, "at", nextnode1(env->stck), env->stck)

PRIVATE void choice_(pEnv env)
{
    THREEPARAMS("choice");
    if (nodevalue(nextnode2(env->stck)).num)
        env->stck = newnode(env, nodetype(nextnode1(env->stck)),
            nodevalue(nextnode1(env->stck)), nextnode3(env->stck));
    else
        env->stck = newnode(env, nodetype(env->stck), nodevalue(env->stck),
            nextnode3(env->stck));
}

PRIVATE void case_(pEnv env)
{
    Index n;
    int error;

    TWOPARAMS("case");
    LIST("case");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "case");
    while (nextnode1(n) != NULL
        && nodevalue(nodevalue(n).lis).num
            != nodevalue(nextnode1(env->stck)).num) {
        if (!Compare(env, nodevalue(n).lis, nextnode1(env->stck), &error)
            && !error)
            break;
        n = nextnode1(n);
    }
    if (nextnode1(n) != NULL) {
        env->stck = nextnode2(env->stck);
        exeterm(env, nextnode1(nodevalue(n).lis));
    } else {
        env->stck = nextnode1(env->stck);
        exeterm(env, nodevalue(n).lis);
    }
}

PRIVATE void opcase_(pEnv env)
{
    Index n;
    ONEPARAM("opcase");
    LIST("opcase");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "opcase");
    while (nextnode1(n) != NULL && nodetype(n) == LIST_
        && nodetype(nodevalue(n).lis) != nodetype(nextnode1(env->stck)))
        n = nextnode1(n);
    CHECKLIST(nodetype(n), "opcase");
    UNARY(LIST_NEWNODE,
        nextnode1(n) != NULL ? nextnode1(nodevalue(n).lis) : nodevalue(n).lis);
}

#define CONS_SWONS(PROCEDURE, NAME, AGGR, ELEM)                                \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        switch (nodetype(AGGR)) {                                              \
        case LIST_:                                                            \
            BINARY(LIST_NEWNODE, newnode(env, nodetype(ELEM), nodevalue(ELEM), \
                                     nodevalue(AGGR).lis));                    \
            break;                                                             \
        case SET_:                                                             \
            CHECKSETMEMBER(ELEM, NAME);                                        \
            BINARY(SET_NEWNODE,                                                \
                nodevalue(AGGR).set | ((long_t)1 << nodevalue(ELEM).num));     \
            break;                                                             \
        case STRING_: {                                                        \
            char *s;                                                           \
            if (nodetype(ELEM) != CHAR_)                                       \
                execerror(env, "character", NAME);                             \
            s = (char *)GC_malloc_atomic(strlen(nodevalue(AGGR).str) + 2);     \
            s[0] = (char)nodevalue(ELEM).num;                                  \
            strcpy(s + 1, nodevalue(AGGR).str);                                \
            BINARY(STRING_NEWNODE, s);                                         \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }

CONS_SWONS(cons_, "cons", env->stck, nextnode1(env->stck))
CONS_SWONS(swons_, "swons", nextnode1(env->stck), env->stck)

PRIVATE void drop_(pEnv env)
{
    int n = nodevalue(env->stck).num;
    TWOPARAMS("drop");
    switch (nodetype(nextnode1(env->stck))) {
    case SET_: {
        int i;
        long_t result = 0;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(nextnode1(env->stck)).set & ((long_t)1 << i)) {
                if (n < 1)
                    result |= ((long_t)1 << i);
                else
                    n--;
            }
        BINARY(SET_NEWNODE, result);
        return;
    }
    case STRING_: {
        char *result = nodevalue(nextnode1(env->stck)).str;
        while (n-- > 0 && *result != '\0')
            ++result;
        BINARY(STRING_NEWNODE, GC_strdup(result));
        return;
    }
    case LIST_: {
        Index result = nodevalue(nextnode1(env->stck)).lis;
        while (n-- > 0 && result != NULL)
            result = nextnode1(result);
        BINARY(LIST_NEWNODE, result);
        return;
    }
    default:
        BADAGGREGATE("drop");
    }
}

PRIVATE void take_(pEnv env)
{
    int n = nodevalue(env->stck).num;

    TWOPARAMS("take");
    switch (nodetype(nextnode1(env->stck))) {
    case SET_: {
        int i;
        long_t result = 0;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(nextnode1(env->stck)).set & ((long_t)1 << i)) {
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
        i = nodevalue(env->stck).num;
        old = nodevalue(nextnode1(env->stck)).str;
        POP(env->stck);
        /* do not swap the order of the next two statements ! ! ! */
        if (i < 0)
            i = 0;
        if ((size_t)i >= strlen(old))
            return; /* the old string unchanged */
        p = result = (char *)GC_malloc_atomic(i + 1);
        while (i-- > 0)
            *p++ = *old++;
        *p = 0;
        UNARY(STRING_NEWNODE, result);
        return;
    }
    case LIST_: {
        int i = nodevalue(env->stck).num;
        if (i < 1) {
            BINARY(LIST_NEWNODE, NULL);
            return;
        } /* null string */
        env->dump1 = newnode(
            env, LIST_, nodevalue(nextnode1(env->stck)), env->dump1); /* old  */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last */
        while (DMP1 != NULL && i-- > 0) {
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                nextnode1(DMP3)
                    = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP3 = nextnode1(DMP3);
            }
            DMP1 = nextnode1(DMP1);
        }
        nextnode1(DMP3) = NULL;
        BINARY(LIST_NEWNODE, DMP2);
        POP(env->dump1);
        POP(env->dump2);
        POP(env->dump3);
        return;
    }
    default:
        BADAGGREGATE("take");
    }
}

PRIVATE void concat_(pEnv env)
{
    TWOPARAMS("concat");
    SAME2TYPES("concat");
    switch (nodetype(env->stck)) {
    case SET_:
        BINARY(SET_NEWNODE,
            nodevalue(nextnode1(env->stck)).set | nodevalue(env->stck).set);
        return;
    case STRING_: {
        char *s, *p;
        s = p = (char *)GC_malloc_atomic(
            strlen(nodevalue(nextnode1(env->stck)).str)
            + strlen(nodevalue(env->stck).str) + 1);
        strcpy(s, nodevalue(nextnode1(env->stck)).str);
        strcat(s, nodevalue(env->stck).str);
        BINARY(STRING_NEWNODE, s);
        return;
    }
    case LIST_:
        if (nodevalue(nextnode1(env->stck)).lis == NULL) {
            BINARY(LIST_NEWNODE, nodevalue(env->stck).lis);
            return;
        }
        env->dump1 = LIST_NEWNODE(
            nodevalue(nextnode1(env->stck)).lis, env->dump1); /* old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last */
        while (DMP1 != NULL) {
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                nextnode1(DMP3)
                    = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP3 = nextnode1(DMP3);
            };
            DMP1 = nextnode1(DMP1);
        }
        nextnode1(DMP3) = nodevalue(env->stck).lis;
        BINARY(LIST_NEWNODE, DMP2);
        POP(env->dump1);
        POP(env->dump2);
        POP(env->dump3);
        return;
    default:
        BADAGGREGATE("concat");
    }
}

PRIVATE void enconcat_(pEnv env)
{
    THREEPARAMS("enconcat");
    SAME2TYPES("enconcat");
    swapd_(env);
    cons_(env);
    concat_(env);
}

PRIVATE void null_(pEnv env)
{
    ONEPARAM("null");
    switch (nodetype(env->stck)) {
    case STRING_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(*(nodevalue(env->stck).str) == '\0'));
        break;
    case FLOAT_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(nodevalue(env->stck).dbl == 0.0));
        break;
    case FILE_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(nodevalue(env->stck).fil == NULL));
        break;
    case LIST_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!nodevalue(env->stck).lis));
        break;
    case SET_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!nodevalue(env->stck).set));
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!nodevalue(env->stck).num));
        break;
    default:
        BADDATA("null");
    }
}

PRIVATE void not_(pEnv env)
{
    ONEPARAM("not");
    switch (nodetype(env->stck)) {
    case SET_:
        UNARY(SET_NEWNODE, ~nodevalue(env->stck).set);
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        UNARY(BOOLEAN_NEWNODE, (long_t)(!nodevalue(env->stck).num));
        break;
    default:
        BADDATA("not");
    }
}

PRIVATE void size_(pEnv env)
{
    long_t siz = 0;
    ONEPARAM("size");
    switch (nodetype(env->stck)) {
    case SET_: {
        int i;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(env->stck).set & ((long_t)1 << i))
                siz++;
        break;
    }
    case STRING_:
        siz = strlen(nodevalue(env->stck).str);
        break;
    case LIST_: {
        Index e = nodevalue(env->stck).lis;
        while (e != NULL) {
            e = nextnode1(e);
            siz++;
        };
        break;
    }
    default:
        BADAGGREGATE("size");
    }
    UNARY(INTEGER_NEWNODE, siz);
}

PRIVATE void small_(pEnv env)
{
    int sml = 0;
    ONEPARAM("small");
    switch (nodetype(env->stck)) {
    case BOOLEAN_:
    case INTEGER_:
        sml = nodevalue(env->stck).num < 2;
        break;
    case SET_:
        if (nodevalue(env->stck).set == 0)
            sml = 1;
        else {
            int i = 0;
            while (!(nodevalue(env->stck).set & ((long_t)1 << i)))
                i++;
            sml = (nodevalue(env->stck).set & ~((long_t)1 << i)) == 0;
        }
        break;
    case STRING_:
        sml = nodevalue(env->stck).str[0] == '\0'
            || nodevalue(env->stck).str[1] == '\0';
        break;
    case LIST_:
        sml = nodevalue(env->stck).lis == NULL
            || nextnode1(nodevalue(env->stck).lis) == NULL;
        break;
    default:
        BADDATA("small");
    }
    UNARY(BOOLEAN_NEWNODE, sml);
}

#define TYPE(PROCEDURE, NAME, REL, TYP)                                        \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        UNARY(BOOLEAN_NEWNODE, (long_t)(nodetype(env->stck) REL TYP));         \
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
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        TYPE(NAME);                                                            \
        BODY;                                                                  \
        POP(env->stck);                                                        \
    }

USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout); printf(" "))
USETOP(
    putch_, "putch", NUMERICTYPE, printf("%c", (char)nodevalue(env->stck).num))
USETOP(putchars_, "putchars", STRING, printf("%s", nodevalue(env->stck).str))
USETOP(setecho_, "setecho", NUMERICTYPE, echoflag = nodevalue(env->stck).num)
USETOP(
    setautoput_, "setautoput", NUMERICTYPE, autoput = nodevalue(env->stck).num)
USETOP(setundeferror_, "setundeferror", NUMERICTYPE,
    undeferror = nodevalue(env->stck).num)
USETOP(
    settracegc_, "settracegc", NUMERICTYPE, tracegc = nodevalue(env->stck).num)
USETOP(srand_, "srand", INTEGER, my_srand((unsigned)nodevalue(env->stck).num))
USETOP(include_, "include", STRING, doinclude(env, nodevalue(env->stck).str))
USETOP(system_, "system", STRING, (void)system(nodevalue(env->stck).str))

PRIVATE void undefs_(pEnv env)
{
    pEntry i = symtabindex;
    Entry ent;

    env->dump1 = LIST_NEWNODE(NULL, env->dump1);
    while (i) {
        ent = vec_at(env->symtab, --i);
        if ((ent.name[0] != 0) && (ent.name[0] != '_') && (ent.u.body == NULL))
            DMP1 = STRING_NEWNODE(ent.name, DMP1);
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
}

PRIVATE void argv_(pEnv env)
{
    int i;
    env->dump1 = LIST_NEWNODE(NULL, env->dump1);
    for (i = g_argc - 1; i >= 0; i--) {
        DMP1 = STRING_NEWNODE(g_argv[i], DMP1);
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
    return;
}

PRIVATE void get_(pEnv env)
{
    getsym(env);
    readfactor(env, 0);
}

PUBLIC void dummy_(pEnv env) { /* never called */}

#define HELP(PROCEDURE, REL)                                                   \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        pEntry i = symtabindex;                                                \
        int column = 0;                                                        \
        int name_length;                                                       \
        Entry ent;                                                             \
        while (i) {                                                            \
            ent = vec_at(env->symtab, --i);                                    \
            if (ent.name[0] REL '_' && !ent.is_local) {                        \
                name_length = strlen(ent.name) + 1;                            \
                if (column + name_length > 72) {                               \
                    printf("\n");                                              \
                    column = 0;                                                \
                }                                                              \
                printf("%s ", ent.name);                                       \
                column += name_length;                                         \
            }                                                                  \
        }                                                                      \
        printf("\n");                                                          \
    }

HELP(help1_, !=)
HELP(h_help1_, ==)

/* - - - - -   C O M B I N A T O R S   - - - - - */

#ifdef TRACING
PUBLIC void printfactor(pEnv env, Index n, FILE *stm)
{
    switch (nodetype(n)) {
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
        fprintf(stm, "%s", env->symtab[(int)nodetype(n)].name);
        return;
    }
}
#endif

#ifdef TRACK_USED_SYMBOLS
static void report_symbols(void)
{
    Entry *n;

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

PUBLIC void exeterm(pEnv env, Index n)
{
    Entry ent;
    Index stepper;

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
    env->conts = LIST_NEWNODE(n, env->conts);
    while (nodevalue(env->conts).lis != NULL) {
#ifdef ENABLE_TRACEGC
        if (tracegc > 5) {
            printf("exeterm1: %d ", nodevalue(env->conts).lis);
            printnode(env, nodevalue(env->conts).lis);
        }
#endif
        stepper = nodevalue(env->conts).lis;
        nodevalue(env->conts).lis = nextnode1(nodevalue(env->conts).lis);
#ifdef STATS
        ++opers;
#endif
#ifdef TRACING
        printfactor(stepper, stdout);
        printf(" . ");
        writeterm(env, env->stck, stdout);
        printf("\n");
#endif
        switch (nodetype(stepper)) {
        case ILLEGAL_:
        case COPIED_:
            printf("exeterm: attempting to execute bad node\n");
#ifdef ENABLE_TRACEGC
            printnode(env, stepper);
#endif
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
            env->stck = newnode(
                env, nodetype(stepper), nodevalue(stepper), env->stck);
            break;
        case USR_:
            ent = vec_at(env->symtab, nodevalue(stepper).ent);
            if (!ent.u.body && undeferror)
                execerror(env, "definition", ent.name);
            if (!nextnode1(stepper)) {
                POP(env->conts);
                n = ent.u.body;
                goto start;
            }
            exeterm(env, ent.u.body);
            break;
        default:
            (*nodevalue(stepper).proc)(env);
#ifdef TRACK_USED_SYMBOLS
            symtab[stepper->op].is_used = 1;
#endif
            break;
        }
#ifdef ENABLE_TRACEGC
        if (tracegc > 5) {
            printf("exeterm2: %d ", stepper);
            printnode(env, stepper);
        }
#endif
    }
    POP(env->conts);
}

PRIVATE void x_(pEnv env)
{
    ONEPARAM("x");
    ONEQUOTE("x");
    exeterm(env, nodevalue(env->stck).lis);
}

PRIVATE void i_(pEnv env)
{
    ONEPARAM("i");
    ONEQUOTE("i");
    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}

PRIVATE void dip_(pEnv env)
{
    TWOPARAMS("dip");
    ONEQUOTE("dip");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}

#define N_ARY(PROCEDURE, NAME, PARAMCOUNT, TOP)                                \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        PARAMCOUNT(NAME);                                                      \
        ONEQUOTE(NAME);                                                        \
        SAVESTACK;                                                             \
        POP(env->stck);                                                        \
        exeterm(env, nodevalue(SAVED1).lis);                                   \
        if (env->stck == NULL)                                                 \
            execerror(env, "value to push", NAME);                             \
        env->stck                                                              \
            = newnode(env, nodetype(env->stck), nodevalue(env->stck), TOP);    \
        POP(env->dump);                                                        \
    }

N_ARY(nullary_, "nullary", ONEPARAM, SAVED2)
N_ARY(unary_, "unary", TWOPARAMS, SAVED3)
N_ARY(binary_, "binary", THREEPARAMS, SAVED4)
N_ARY(ternary_, "ternary", FOURPARAMS, SAVED5)

PRIVATE void times_(pEnv env)
{
    int i, n;

    TWOPARAMS("times");
    ONEQUOTE("times");
    INTEGER2("times");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    n = nodevalue(SAVED2).num;
    for (i = 1; i <= n; i++)
        exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}

PRIVATE void infra_(pEnv env)
{
    TWOPARAMS("infra");
    ONEQUOTE("infra");
    LIST2("infra");
    SAVESTACK;
    env->stck = nodevalue(SAVED2).lis;
    exeterm(env, nodevalue(SAVED1).lis);
    env->stck = LIST_NEWNODE(env->stck, SAVED3);
    POP(env->dump);
}

PRIVATE void app1_(pEnv env)
{
    TWOPARAMS("app1");
    ONEQUOTE("app1");
    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}

PRIVATE void cleave_(pEnv env)
{ /*  X [P1] [P2] cleave ==>  X1 X2        */
    THREEPARAMS("cleave");
    TWOQUOTES("cleave");
    SAVESTACK;
    env->stck = SAVED3;
    exeterm(env, nodevalue(SAVED2).lis); /* [P1]                */
    env->dump1 = newnode(
        env, nodetype(env->stck), nodevalue(env->stck), env->dump1); /*  X1 */
    env->stck = SAVED3;
    exeterm(env, nodevalue(SAVED1).lis); /* [P2]                */
    env->dump1 = newnode(
        env, nodetype(env->stck), nodevalue(env->stck), env->dump1); /*  X2 */
    env->stck = env->dump1;
    env->dump1 = nextnode2(env->dump1);
    nextnode2(env->stck) = SAVED4;
    POP(env->dump);
}

PRIVATE void app11_(pEnv env)
{
    THREEPARAMS("app11");
    ONEQUOTE("app11");
    app1_(env);
    nextnode1(env->stck) = nextnode2(env->stck);
}

PRIVATE void unary2_(pEnv env)
{ /*   Y  Z  [P]  unary2     ==>  Y'  Z'  */
    THREEPARAMS("unary2");
    ONEQUOTE("unary2");
    SAVESTACK;
    env->stck = nextnode1(SAVED2); /* just Y on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Y) */
    env->stck = newnode(env, nodetype(SAVED2), nodevalue(SAVED2),
        nextnode1(SAVED3)); /* just Z on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Z) */
    env->stck = env->dump1;
    env->dump1 = nextnode2(env->dump1);
    nextnode2(env->stck) = SAVED4;
    POP(env->dump);
}

PRIVATE void unary3_(pEnv env)
{ /*  X Y Z [P]  unary3    ==>  X' Y' Z'        */
    FOURPARAMS("unary3");
    ONEQUOTE("unary3");
    SAVESTACK;
    env->stck = nextnode1(SAVED3); /* just X on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(X) */
    env->stck = newnode(env, nodetype(SAVED3), nodevalue(SAVED3),
        nextnode1(SAVED4)); /* just Y on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Y) */
    env->stck = newnode(env, nodetype(SAVED2), nodevalue(SAVED2),
        nextnode1(SAVED4)); /* just Z on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Z) */
    env->stck = env->dump1;
    env->dump1 = nextnode3(env->dump1);
    nextnode3(env->stck) = SAVED5;
    POP(env->dump);
}

PRIVATE void unary4_(pEnv env)
{ /*  X Y Z W [P]  unary4    ==>  X' Y' Z' W'        */
    FIVEPARAMS("unary4");
    ONEQUOTE("unary4");
    SAVESTACK;
    env->stck = nextnode1(SAVED4); /* just X on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save p(X) */
    env->stck = newnode(env, nodetype(SAVED4), nodevalue(SAVED4),
        nextnode1(SAVED5)); /* just Y on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Y) */
    env->stck = newnode(env, nodetype(SAVED3), nodevalue(SAVED3),
        nextnode1(SAVED5)); /* just Z on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Z) */
    env->stck = newnode(env, nodetype(SAVED2), nodevalue(SAVED2),
        nextnode1(SAVED5)); /* just W on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(W) */
    env->stck = env->dump1;
    env->dump1 = nextnode4(env->dump1);
    nextnode4(env->stck) = SAVED6;
    POP(env->dump);
}

PRIVATE void app12_(pEnv env)
{
    /*   X  Y  Z  [P]  app12  */
    THREEPARAMS("app12");
    unary2_(env);
    nextnode2(env->stck) = nextnode3(env->stck); /* delete X */
}

PRIVATE void map_(pEnv env)
{
    TWOPARAMS("map");
    ONEQUOTE("map");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case LIST_: {
        env->dump1
            = newnode(env, LIST_, nodevalue(SAVED2), env->dump1); /* step old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head new */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last new */
        while (DMP1 != NULL) {
            env->stck = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (env->stck == NULL)
                execerror(env, "non-empty stack", "map");
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(nodetype(env->stck), nodevalue(env->stck), NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                nextnode1(DMP3)
                    = NEWNODE(nodetype(env->stck), nodevalue(env->stck), NULL);
                DMP3 = nextnode1(DMP3);
            }
            DMP1 = nextnode1(DMP1);
        }
        env->stck = LIST_NEWNODE(DMP2, SAVED3);
        POP(env->dump3);
        POP(env->dump2);
        POP(env->dump1);
        break;
    }
    case STRING_: {
        char *s, *resultstring;
        int j = 0;
        resultstring
            = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            resultstring[j++] = (char)nodevalue(env->stck).num;
        }
        resultstring[j] = '\0';
        env->stck = STRING_NEWNODE(resultstring, SAVED3);
        break;
    }
    case SET_: {
        int i;
        long_t resultset = 0;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(SAVED2).set & ((long_t)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                resultset |= ((long_t)1 << nodevalue(env->stck).num);
            }
        env->stck = SET_NEWNODE(resultset, SAVED3);
        break;
    }
    default:
        BADAGGREGATE("map");
    }
    POP(env->dump);
}

PRIVATE void step_(pEnv env)
{
    TWOPARAMS("step");
    ONEQUOTE("step");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    switch (nodetype(SAVED2)) {
    case LIST_: {
        env->dump1 = newnode(env, LIST_, nodevalue(SAVED2), env->dump1);
        while (DMP1 != NULL) {
            GNULLARY(nodetype(DMP1), nodevalue(DMP1));
            exeterm(env, nodevalue(SAVED1).lis);
            DMP1 = nextnode1(DMP1);
        }
        POP(env->dump1);
        break;
    }
    case STRING_: {
        char *s;
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long_t)*s, env->stck);
            exeterm(env, nodevalue(SAVED1).lis);
        }
        break;
    }
    case SET_: {
        int i;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(SAVED2).set & ((long_t)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, env->stck);
                exeterm(env, nodevalue(SAVED1).lis);
            }
        break;
    }
    default:
        BADAGGREGATE("step");
    }
    POP(env->dump);
}

PRIVATE void fold_(pEnv env)
{
    THREEPARAMS("fold");
    swapd_(env);
    step_(env);
}

PRIVATE void cond_(pEnv env)
{
    Index my_dump;
    int result = 0;

    ONEPARAM("cond");
    LIST("cond");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "cond");
    /* must check for QUOTES in list */
    for (my_dump = nodevalue(env->stck).lis; nextnode1(my_dump);
         my_dump = nextnode1(my_dump))
        CHECKLIST(nodetype(nodevalue(my_dump).lis), "cond");
    SAVESTACK;
    env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
    while (result == 0 && DMP1 != NULL && nextnode1(DMP1) != NULL) {
        env->stck = SAVED2;
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        result = nodevalue(env->stck).num;
        if (!result)
            DMP1 = nextnode1(DMP1);
    }
    env->stck = SAVED2;
    if (result)
        exeterm(env, nextnode1(nodevalue(DMP1).lis));
    else
        exeterm(env, nodevalue(DMP1).lis); /* default */
    POP(env->dump1);
    POP(env->dump);
}

#define IF_TYPE(PROCEDURE, NAME, TYP)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        TWOQUOTES(NAME);                                                       \
        SAVESTACK;                                                             \
        env->stck = SAVED3;                                                    \
        exeterm(env, nodetype(env->stck) == TYP ? nodevalue(SAVED2).lis        \
                                                : nodevalue(SAVED1).lis);      \
        POP(env->dump);                                                        \
    }

IF_TYPE(ifinteger_, "ifinteger", INTEGER_)
IF_TYPE(ifchar_, "ifchar", CHAR_)
IF_TYPE(iflogical_, "iflogical", BOOLEAN_)
IF_TYPE(ifstring_, "ifstring", STRING_)
IF_TYPE(ifset_, "ifset", SET_)
IF_TYPE(iffloat_, "iffloat", FLOAT_)
IF_TYPE(iffile_, "iffile", FILE_)
IF_TYPE(iflist_, "iflist", LIST_)

PRIVATE void filter_(pEnv env)
{
    TWOPARAMS("filter");
    ONEQUOTE("filter");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_: {
        int j;
        long_t resultset = 0;
        for (j = 0; j < SETSIZE; j++) {
            if (nodevalue(SAVED2).set & ((long_t)1 << j)) {
                env->stck = INTEGER_NEWNODE(j, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                if (nodevalue(env->stck).num)
                    resultset |= ((long_t)1 << j);
            }
        }
        env->stck = SET_NEWNODE(resultset, SAVED3);
        break;
    }
    case STRING_: {
        char *s, *resultstring;
        int j = 0;
        resultstring
            = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (nodevalue(env->stck).num)
                resultstring[j++] = *s;
        }
        resultstring[j] = '\0';
        env->stck = STRING_NEWNODE(resultstring, SAVED3);
        break;
    }
    case LIST_: {
        env->dump1
            = newnode(env, LIST_, nodevalue(SAVED2), env->dump1); /* step old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head new */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last new */
        while (DMP1 != NULL) {
            env->stck = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (nodevalue(env->stck).num) /* test */
            {
                if (DMP2 == NULL) /* first */
                {
                    DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                    DMP3 = DMP2;
                } else /* further */
                {
                    nextnode1(DMP3)
                        = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                    DMP3 = nextnode1(DMP3);
                }
            }
            DMP1 = nextnode1(DMP1);
        }
        env->stck = LIST_NEWNODE(DMP2, SAVED3);
        POP(env->dump3);
        POP(env->dump2);
        POP(env->dump1);
        break;
    }
    default:
        BADAGGREGATE("filter");
    }
    POP(env->dump);
}

PRIVATE void split_(pEnv env)
{
    TWOPARAMS("split");
    ONEQUOTE("split");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_: {
        int j;
        long_t yes_set = 0, no_set = 0;
        for (j = 0; j < SETSIZE; j++) {
            if (nodevalue(SAVED2).set & ((long_t)1 << j)) {
                env->stck = INTEGER_NEWNODE(j, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                if (nodevalue(env->stck).num)
                    yes_set |= ((long_t)1 << j);
                else
                    no_set |= ((long_t)1 << j);
            }
        }
        env->stck = SET_NEWNODE(yes_set, SAVED3);
        NULLARY(SET_NEWNODE, no_set);
        break;
    }
    case STRING_: {
        char *s, *yesstring, *nostring;
        int yesptr = 0, noptr = 0;
        yesstring = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        nostring = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (nodevalue(env->stck).num)
                yesstring[yesptr++] = *s;
            else
                nostring[noptr++] = *s;
        }
        yesstring[yesptr] = '\0';
        nostring[noptr] = '\0';
        env->stck = STRING_NEWNODE(yesstring, SAVED3);
        NULLARY(STRING_NEWNODE, nostring);
        break;
    }
    case LIST_: {
        env->dump1
            = newnode(env, LIST_, nodevalue(SAVED2), env->dump1); /* step old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head true */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last true */
        env->dump4 = LIST_NEWNODE(0L, env->dump4); /* head false */
        env->dump5 = LIST_NEWNODE(0L, env->dump5); /* last false */
        while (DMP1 != NULL) {
            env->stck = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (nodevalue(env->stck).num) /* pass */
                if (DMP2 == NULL) /* first */
                {
                    DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                    DMP3 = DMP2;
                } else /* further */
                {
                    nextnode1(DMP3)
                        = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                    DMP3 = nextnode1(DMP3);
                }
            else /* fail */
                if (DMP4 == NULL) /* first */
            {
                DMP4 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP5 = DMP4;
            } else /* further */
            {
                nextnode1(DMP5)
                    = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP5 = nextnode1(DMP5);
            }
            DMP1 = nextnode1(DMP1);
        }
        env->stck = LIST_NEWNODE(DMP2, SAVED3);
        NULLARY(LIST_NEWNODE, DMP4);
        POP(env->dump5);
        POP(env->dump4);
        POP(env->dump3);
        POP(env->dump2);
        POP(env->dump1);
        break;
    }
    default:
        BADAGGREGATE("split");
    }
    POP(env->dump);
}

#define SOMEALL(PROCEDURE, NAME, INITIAL)                                      \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        int result = INITIAL;                                                  \
        TWOPARAMS(NAME);                                                       \
        ONEQUOTE(NAME);                                                        \
        SAVESTACK;                                                             \
        switch (nodetype(SAVED2)) {                                            \
        case SET_: {                                                           \
            int j;                                                             \
            for (j = 0; j < SETSIZE && result == INITIAL; j++) {               \
                if (nodevalue(SAVED2).set & ((long_t)1 << j)) {                \
                    env->stck = INTEGER_NEWNODE(j, SAVED3);                    \
                    exeterm(env, nodevalue(SAVED1).lis);                       \
                    if (nodevalue(env->stck).num != INITIAL)                   \
                        result = 1 - INITIAL;                                  \
                }                                                              \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        case STRING_: {                                                        \
            char *s;                                                           \
            for (s = nodevalue(SAVED2).str; *s != '\0' && result == INITIAL;   \
                 s++) {                                                        \
                env->stck = CHAR_NEWNODE((long_t)*s, SAVED3);                  \
                exeterm(env, nodevalue(SAVED1).lis);                           \
                if (nodevalue(env->stck).num != INITIAL)                       \
                    result = 1 - INITIAL;                                      \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            env->dump1 = newnode(env, LIST_, nodevalue(SAVED2), env->dump1);   \
            while (DMP1 != NULL && result == INITIAL) {                        \
                env->stck                                                      \
                    = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);   \
                exeterm(env, nodevalue(SAVED1).lis);                           \
                if (nodevalue(env->stck).num != INITIAL)                       \
                    result = 1 - INITIAL;                                      \
                DMP1 = nextnode1(DMP1);                                        \
            }                                                                  \
            POP(env->dump1);                                                   \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        env->stck = BOOLEAN_NEWNODE(result, SAVED3);                           \
        POP(env->dump);                                                        \
    }

SOMEALL(some_, "some", 0L)
SOMEALL(all_, "all", 1L)

PRIVATE void primrec_(pEnv env)
{
    int n = 0;
    int i;
    THREEPARAMS("primrec");
    TWOQUOTES("primrec");
    SAVESTACK;
    env->stck = nextnode3(env->stck);
    switch (nodetype(SAVED3)) {
    case LIST_: {
        env->dump1 = newnode(env, LIST_, nodevalue(SAVED3), env->dump1);
        while (DMP1 != NULL) {
            env->stck
                = newnode(env, nodetype(DMP1), nodevalue(DMP1), env->stck);
            DMP1 = nextnode1(DMP1);
            n++;
        }
        POP(env->dump1);
        break;
    }
    case STRING_: {
        char *s;
        for (s = nodevalue(SAVED3).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long_t)*s, env->stck);
            n++;
        }
        break;
    }
    case SET_: {
        int j;
        long_t set = nodevalue(SAVED3).set;
        for (j = 0; j < SETSIZE; j++)
            if (set & ((long_t)1 << j)) {
                env->stck = INTEGER_NEWNODE(j, env->stck);
                n++;
            }
        break;
    }
    case INTEGER_: {
        long_t j;
        for (j = nodevalue(SAVED3).num; j > 0; j--) {
            env->stck = INTEGER_NEWNODE(j, env->stck);
            n++;
        }
        break;
    }
    default:
        BADDATA("primrec");
    }
    exeterm(env, nodevalue(SAVED2).lis);
    for (i = 1; i <= n; i++)
        exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}

PRIVATE void tailrecaux(pEnv env)
{
    int result;
tailrec:
    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED3).lis);
    result = nodevalue(env->stck).num;
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
        exeterm(env, nodevalue(SAVED2).lis);
    else {
        exeterm(env, nodevalue(SAVED1).lis);
        goto tailrec;
    }
}

PRIVATE void tailrec_(pEnv env)
{
    THREEPARAMS("tailrec");
    THREEQUOTES("tailrec");
    SAVESTACK;
    env->stck = SAVED4;
    tailrecaux(env);
    POP(env->dump);
}

PRIVATE void construct_(pEnv env)
{ /* [P] [[P1] [P2] ..] -> X1 X2 ..        */
    TWOPARAMS("construct");
    TWOQUOTES("construct");
    SAVESTACK;
    env->stck = SAVED3; /* pop progs                */
    env->dump1 = LIST_NEWNODE(env->dump2, env->dump1); /* save env->dump2 */
    env->dump2 = env->stck; /* save old stack        */
    exeterm(env, nodevalue(SAVED2).lis); /* [P]                        */
    env->dump3 = LIST_NEWNODE(env->stck, env->dump3); /* save current stack */
    env->dump4
        = newnode(env, LIST_, nodevalue(SAVED1), env->dump4); /* step [..] */
    while (DMP4 != NULL) {
        env->stck = DMP3; /* restore new stack        */
        exeterm(env, nodevalue(DMP4).lis);
        env->dump2 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
            env->dump2); /* result        */
        DMP4 = nextnode1(DMP4);
    }
    POP(env->dump4);
    POP(env->dump3);
    env->stck = env->dump2;
    env->dump2 = nodevalue(env->dump1).lis; /* restore env->dump2        */
    POP(env->dump1);
    POP(env->dump);
}

PRIVATE void branch_(pEnv env)
{
    THREEPARAMS("branch");
    TWOQUOTES("branch");
    SAVESTACK;
    env->stck = SAVED4;
    exeterm(env,
        nodevalue(SAVED3).num ? nodevalue(SAVED2).lis : nodevalue(SAVED1).lis);
    POP(env->dump);
}

PRIVATE void while_(pEnv env)
{
    TWOPARAMS("while");
    TWOQUOTES("while");
    SAVESTACK;
    do {
        env->stck = SAVED3;
        exeterm(env, nodevalue(SAVED2).lis); /* TEST */
        if (!nodevalue(env->stck).num)
            break;
        env->stck = SAVED3;
        exeterm(env, nodevalue(SAVED1).lis); /* DO */
        SAVED3 = env->stck;
    } while (1);
    env->stck = SAVED3;
    POP(env->dump);
}

PRIVATE void ifte_(pEnv env)
{
    int result;
    THREEPARAMS("ifte");
    THREEQUOTES("ifte");
    SAVESTACK;
    env->stck = SAVED4;
    exeterm(env, nodevalue(SAVED3).lis);
    result = nodevalue(env->stck).num;
    env->stck = SAVED4;
    exeterm(env, result ? nodevalue(SAVED2).lis : nodevalue(SAVED1).lis);
    POP(env->dump);
}

PRIVATE void condlinrecaux(pEnv env)
{
    int result = 0;
    env->dump1 = newnode(env, LIST_, nodevalue(SAVED1), env->dump1);
    env->dump2 = LIST_NEWNODE(env->stck, env->dump2);
    while (result == 0 && DMP1 != NULL && nextnode1(DMP1) != NULL) {
        env->stck = DMP2;
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        result = nodevalue(env->stck).num;
        if (!result)
            DMP1 = nextnode1(DMP1);
    }
    env->stck = DMP2;
    if (result) {
        exeterm(env, nodevalue(nextnode1(nodevalue(DMP1).lis)).lis);
        if (nextnode2(nodevalue(DMP1).lis) != NULL) {
            condlinrecaux(env);
            exeterm(env, nodevalue(nextnode2(nodevalue(DMP1).lis)).lis);
        }
    } else {
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        if (nextnode1(nodevalue(DMP1).lis) != NULL) {
            condlinrecaux(env);
            exeterm(env, nodevalue(nextnode1(nodevalue(DMP1).lis)).lis);
        }
    }
    POP(env->dump2);
    POP(env->dump1);
}

PRIVATE void condlinrec_(pEnv env)
{
    ONEPARAM("condlinrec");
    LIST("condlinrec");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "condlinrec");
    SAVESTACK;
    env->stck = SAVED2;
    condlinrecaux(env);
    POP(env->dump);
}

PRIVATE void condnestrecaux(pEnv env)
{
    int result = 0;
    env->dump1 = newnode(env, LIST_, nodevalue(SAVED1), env->dump1);
    env->dump2 = LIST_NEWNODE(env->stck, env->dump2);
    while (result == 0 && DMP1 != NULL && nextnode1(DMP1) != NULL) {
        env->stck = DMP2;
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        result = nodevalue(env->stck).num;
        if (!result)
            DMP1 = nextnode1(DMP1);
    }
    env->stck = DMP2;
    env->dump3 = LIST_NEWNODE(
        (result ? nextnode1(nodevalue(DMP1).lis) : nodevalue(DMP1).lis),
        env->dump3);
    exeterm(env, nodevalue(DMP3).lis);
    DMP3 = nextnode1(DMP3);
    while (DMP3 != NULL) {
        condnestrecaux(env);
        exeterm(env, nodevalue(DMP3).lis);
        DMP3 = nextnode1(DMP3);
    }
    POP(env->dump3);
    POP(env->dump2);
    POP(env->dump1);
}

PRIVATE void condnestrec_(pEnv env)
{
    ONEPARAM("condnestrec");
    LIST("condnestrec");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "condnestrec");
    SAVESTACK;
    env->stck = SAVED2;
    condnestrecaux(env);
    POP(env->dump);
}

PRIVATE void linrecaux(pEnv env)
{
    int result;
    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED4).lis);
    result = nodevalue(env->stck).num;
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
        exeterm(env, nodevalue(SAVED3).lis);
    else {
        exeterm(env, nodevalue(SAVED2).lis);
        linrecaux(env);
        exeterm(env, nodevalue(SAVED1).lis);
    }
}

PRIVATE void linrec_(pEnv env)
{
    FOURPARAMS("linrec");
    FOURQUOTES("linrec");
    SAVESTACK;
    env->stck = SAVED5;
    linrecaux(env);
    POP(env->dump);
}

PRIVATE void binrecaux(pEnv env)
{
    int result;
    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED4).lis);
    result = nodevalue(env->stck).num;
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
        exeterm(env, nodevalue(SAVED3).lis);
    else {
        exeterm(env, nodevalue(SAVED2).lis); /* split */
        env->dump2 = newnode(
            env, nodetype(env->stck), nodevalue(env->stck), env->dump2);
        POP(env->stck);
        binrecaux(env); /* first */
        GNULLARY(nodetype(env->dump2), nodevalue(env->dump2));
        POP(env->dump2);
        binrecaux(env); /* second */
        exeterm(env, nodevalue(SAVED1).lis);
    } /* combine */
}

PRIVATE void binrec_(pEnv env)
{
    FOURPARAMS("binrec");
    FOURQUOTES("binrec");
    SAVESTACK;
    env->stck = SAVED5;
    binrecaux(env);
    POP(env->dump);
}

PRIVATE void treestepaux(pEnv env, Index item)
{
    if (nodetype(item) != LIST_) {
        GNULLARY(nodetype(item), nodevalue(item));
        exeterm(env, nodevalue(SAVED1).lis);
    } else {
        env->dump1 = newnode(env, LIST_, nodevalue(item), env->dump1);
        while (DMP1 != NULL) {
            treestepaux(env, DMP1);
            DMP1 = nextnode1(DMP1);
        }
        POP(env->dump1);
    }
}

PRIVATE void treestep_(pEnv env)
{
    TWOPARAMS("treestep");
    ONEQUOTE("treestep");
    SAVESTACK;
    env->stck = SAVED3;
    treestepaux(env, SAVED2);
    POP(env->dump);
}

PRIVATE void treerecaux(pEnv env)
{
    if (nodetype(nextnode1(env->stck)) == LIST_) {
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(treerecaux, NULL));
        cons_(env); /*  D  [[[O] C] ANON_FUNCT_]        */
        exeterm(env, nextnode1(nodevalue(nodevalue(env->stck).lis).lis));
    } else {
        env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
        POP(env->stck);
        exeterm(env, nodevalue(DMP1).lis);
        POP(env->dump1);
    }
}

PRIVATE void treerec_(pEnv env)
{
    THREEPARAMS("treerec");
    TWOQUOTES("treerec");
    cons_(env);
    treerecaux(env);
}

PRIVATE void genrecaux(pEnv env)
{
    int result;
    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(nodevalue(SAVED1).lis).lis); /*        [I]        */
    result = nodevalue(env->stck).num;
    env->stck = SAVED2;
    if (result)
        exeterm(env,
            nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis); /*        [T] */
    else {
        exeterm(env,
            nodevalue(nextnode2(nodevalue(SAVED1).lis)).lis); /*        [R1] */
        NULLARY(LIST_NEWNODE, nodevalue(SAVED1).lis);
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(genrecaux, NULL));
        cons_(env);
        exeterm(env, nextnode3(nodevalue(SAVED1).lis));
    } /*   [R2]        */
    POP(env->dump);
}

PRIVATE void genrec_(pEnv env)
{
    FOURPARAMS("genrec");
    FOURQUOTES("genrec");
    cons_(env);
    cons_(env);
    cons_(env);
    genrecaux(env);
}

PRIVATE void treegenrecaux(pEnv env)
{
    if (nodetype(nextnode1(env->stck)) == LIST_) {
        SAVESTACK; /* begin DIP        */
        POP(env->stck);
        exeterm(env,
            nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis); /*        [O2] */
        GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));
        POP(env->dump); /*   end DIP        */
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(treegenrecaux, NULL));
        cons_(env);
        exeterm(env, nextnode2(nodevalue(nodevalue(env->stck).lis).lis));
    } /*        [C]        */
    else {
        env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
        POP(env->stck);
        exeterm(env, nodevalue(DMP1).lis);
        POP(env->dump1);
    }
}

PRIVATE void treegenrec_(pEnv env)
{ /* T [O1] [O2] [C]        */
    FOURPARAMS("treegenrec");
    THREEQUOTES("treegenrec");
    cons_(env);
    cons_(env);
    treegenrecaux(env);
}

PRIVATE void plain_manual_(pEnv env) { make_manual(0); }

PRIVATE void html_manual_(pEnv env) { make_manual(1); }

PRIVATE void latex_manual_(pEnv env) { make_manual(2); }

/* - - - - -   I N I T I A L I S A T I O N   - - - - - */

/* clang-format off */

static struct {
    char* name;
    void (*proc)(pEnv env);
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

/* clang-format on */

PRIVATE void helpdetail_(pEnv env)
{
    Index n;
    Entry ent;

    ONEPARAM("HELP");
    LIST("HELP");
    printf("\n");
    n = nodevalue(env->stck).lis;
    while (n != NULL) {
        if (nodetype(n) == USR_) {
            ent = vec_at(env->symtab, nodevalue(n).ent);
            printf("%s  ==\n    ", ent.name);
            writeterm(env, ent.u.body, stdout);
            printf("\n");
            break;
        } else {
            Operator op;
            if ((op = nodetype(n)) == BOOLEAN_)
                op = nodevalue(n).num ? TRUE_ : FALSE_;
            if (op == INTEGER_ && nodevalue(n).num == MAXINT)
                op = MAXINT_;
            printf("%s	:   %s.\n%s\n", optable[(int)op].name,
                optable[(int)op].messg1, optable[(int)op].messg2);
        }
        printf("\n");
        n = nextnode1(n);
    }
    POP(env->stck);
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
        char *n = optable[i].name;
        /* clang-format off */
        HEADER(n, " truth value type", "literal") else
	HEADER(n, "false", "operand") else
	HEADER(n, "id", "operator") else
	HEADER(n, "null", "predicate") else
	HEADER(n, "i", "combinator") else
	HEADER(n, "help", "miscellaneous commands")
	if (n[0] != '_') {
            /* clang-format on */
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

PRIVATE void manual_list_(pEnv env)
{
    int i = -1;
    while (optable[++i].name)
        ; /* find end */
    --i; /* overshot */
    env->dump1 = LIST_NEWNODE(0, env->dump1);
    env->dump2 = LIST_NEWNODE(0, env->dump2);
    while (i) {
        DMP1 = STRING_NEWNODE(optable[i].messg2, 0);
        DMP1 = STRING_NEWNODE(optable[i].messg1, DMP1);
        DMP1 = STRING_NEWNODE(optable[i].name, DMP1);
        DMP2 = LIST_NEWNODE(DMP1, DMP2);
        --i;
    }
    env->stck = LIST_NEWNODE(DMP2, env->stck);
    POP(env->dump2);
    POP(env->dump1);
}

PUBLIC char *opername(int o) { return optable[o].name; }

PUBLIC void (*operproc(int o))(pEnv) { return optable[o].proc; }
/* END of INTERP.C */
