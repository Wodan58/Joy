/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.54
 *  date    : 04/13/22
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
#include "globals.h"

#define NEWNODE(o, u, r)                                                       \
    (env->bucket.lis = newnode(env, o, u, r), env->bucket.lis)
#define USR_NEWNODE(u, r) (env->bucket.ent = u, NEWNODE(USR_, env->bucket, r))
#define ANON_FUNCT_NEWNODE(u, r)                                               \
    (env->bucket.proc = u, NEWNODE(ANON_FUNCT_, env->bucket, r))
#define BOOLEAN_NEWNODE(u, r)                                                  \
    (env->bucket.num = u, NEWNODE(BOOLEAN_, env->bucket, r))
#define CHAR_NEWNODE(u, r) (env->bucket.num = u, NEWNODE(CHAR_, env->bucket, r))
#define INTEGER_NEWNODE(u, r)                                                  \
    (env->bucket.num = u, NEWNODE(INTEGER_, env->bucket, r))
#define SET_NEWNODE(u, r) (env->bucket.num = u, NEWNODE(SET_, env->bucket, r))
#define STRING_NEWNODE(u, r)                                                   \
    (env->bucket.str = u, NEWNODE(STRING_, env->bucket, r))
#define LIST_NEWNODE(u, r) (env->bucket.lis = u, NEWNODE(LIST_, env->bucket, r))
#define FLOAT_NEWNODE(u, r)                                                    \
    (env->bucket.dbl = u, NEWNODE(FLOAT_, env->bucket, r))
#define FILE_NEWNODE(u, r) (env->bucket.fil = u, NEWNODE(FILE_, env->bucket, r))

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
            && nodetype(nextnode1(env->stck)) == INTEGER_)                     \
        || (nodetype(env->stck) == INTEGER_                                    \
            && nodetype(nextnode1(env->stck)) == FLOAT_))
#define FLOAT(NAME)                                                            \
    if (!FLOATABLE)                                                            \
        execerror(env, "float or integer", NAME);
#define FLOAT2(NAME)                                                           \
    if (!(FLOATABLE2                                                           \
            || (nodetype(env->stck) == INTEGER_                                \
                && nodetype(nextnode1(env->stck)) == INTEGER_)))               \
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
        || nodevalue(NODE).num < 0 || nodevalue(NODE).num >= SETSIZE)          \
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

#ifdef STATS
static double calls, opers;

PRIVATE void report_stats(void)
{
    fprintf(stderr, "%.0f calls to joy interpreter\n", calls);
    fprintf(stderr, "%.0f operations executed\n", opers);
}
#endif

#ifdef TRACING
PRIVATE void writestack(pEnv env, Index n, FILE *stm)
{
    if (n) {
        writestack(env, nextnode1(n), stm);
        if (nextnode1(n))
            fputc(' ', stm);
        writefactor(env, n, stm);
    }
}
#endif

/*
    exeterm starts with n. If during execution n comes up again, the function
    is directly recursive. That is allowed, except when the very first factor
    is also n. In that case there is recursion without an end condition. It is
    possible to discover that specific case.
*/
PUBLIC void exeterm(pEnv env, Index n)
{
    Entry ent;
    int type, index;
    Index stepper, root = 0;

start:
#ifdef STATS
    if (++calls == 1)
        atexit(report_stats);
#endif
    if (root == n)
        return;
    root = n;
#ifdef NOBDW
    env->bucket.lis = n;
    env->conts = newnode(env, LIST_, env->bucket, env->conts);
    while (nodevalue(env->conts).lis) {
#else
    while (n) {
#endif
#ifdef ENABLE_TRACEGC
        if (env->tracegc > 5) {
            printf("exeterm1: %d ", nodevalue(env->conts).lis);
            printnode(env, nodevalue(env->conts).lis);
        }
#endif
#ifdef NOBDW
        stepper = nodevalue(env->conts).lis;
        nodevalue(env->conts).lis = nextnode1(nodevalue(env->conts).lis);
#else
        stepper = n;
#endif
#ifdef STATS
        ++opers;
#endif
        type = opertype(nodetype(stepper));
#ifdef TRACING
        if (env->debugging) {
            writestack(env, env->stck, stdout);
            printf(" : ");
            writeterm(env, stepper, stdout);
            putchar('\n');
        }
#endif
        switch (type) {
        case ILLEGAL_:
        case COPIED_:
            printf("exeterm: attempting to execute bad node\n");
#ifdef ENABLE_TRACEGC
            printnode(env, stepper);
#endif
            return;
        case USR_:
            index = nodevalue(stepper).ent;
            ent = vec_at(env->symtab, index);
            if (!ent.u.body && env->undeferror)
                execerror(env, "definition", ent.name);
            if (!nextnode1(stepper)) {
#ifdef NOBDW
                POP(env->conts);
#endif
                n = ent.u.body;
                goto start;
            }
            if (ent.u.body != root)
                exeterm(env, ent.u.body);
            break;
        case BOOLEAN_:
        case CHAR_:
        case INTEGER_:
        case SET_:
        case STRING_:
        case LIST_:
        case FLOAT_:
        case FILE_:
            env->stck = newnode(env, type, nodevalue(stepper), env->stck);
            break;
            /*
                Builtins have their own type.
            */
        default:
            (*nodevalue(stepper).proc)(env);
            break;
        }
#ifdef ENABLE_TRACEGC
        if (env->tracegc > 5) {
            printf("exeterm2: %d ", stepper);
            printnode(env, stepper);
        }
#endif
#ifndef NOBDW
        n = n->next;
#endif
    }
#ifdef NOBDW
    POP(env->conts);
#endif
}

/* clang-format off */
#include "src/andorxor.h"
#include "src/bfloat.h"
#include "src/comprel.h"
#include "src/cons_swons.h"
#include "src/dipped.h"
#include "src/fileget.h"
#include "src/help.h"
#include "src/if_type.h"
#include "src/inhas.h"
#include "src/maxmin.h"
#include "src/n_ary.h"
#include "src/of_at.h"
#include "src/ordchr.h"
#include "src/plusminus.h"
#include "src/predsucc.h"
#include "src/push.h"
#include "src/someall.h"
#include "src/type.h"
#include "src/ufloat.h"
#include "src/unmktime.h"
#include "src/usetop.h"
#include "builtin.h"

static struct {
    char *name;
    void (*proc)(pEnv env);
    char *messg1, *messg2;
} optable[] = {
    /* THESE MUST BE DEFINED IN THE ORDER OF THEIR VALUES */
{"__ILLEGAL",           id_,            "->",
"internal error, cannot happen - supposedly."},

{"__COPIED",            id_,            "->",
"no message ever, used for gc."},

{"__USR",               id_,            "->",
"user node."},

{"__ANON_FUNCT",        id_,            "->",
"op for anonymous function call."},

/* LITERALS */

{" truth value type",   id_,            "->  B",
"The logical type, or the type of truth values.\nIt has just two literals: true and false."},

{" character type",     id_,            "->  C",
"The type of characters. Literals are written with a single quote.\nExamples:  'A  '7  ';  and so on. Unix style escapes are allowed."},

{" integer type",       id_,            "->  I",
"The type of negative, zero or positive integers.\nLiterals are written in decimal notation. Examples:  -123   0   42."},

{" set type",           id_,            "->  {...}",
"The type of sets of small non-negative integers.\nThe maximum is platform dependent, typically the range is 0..31.\nLiterals are written inside curly braces.\nExamples:  {}  {0}  {1 3 5}  {19 18 17}."},

{" string type",        id_,            "->  \"...\" ",
"The type of strings of characters. Literals are written inside double quotes.\nExamples: \"\"  \"A\"  \"hello world\" \"123\".\nUnix style escapes are accepted."},

{" list type",          id_,            "->  [...]",
"The type of lists of values of any type (including lists),\nor the type of quoted programs which may contain operators or combinators.\nLiterals of this type are written inside square brackets.\nExamples: []  [3 512 -7]  [john mary]  ['A 'C ['B]]  [dup *]."},

{" float type",         id_,            "->  F",
"The type of floating-point numbers.\nLiterals of this type are written with embedded decimal points (like 1.2)\nand optional exponent specifiers (like 1.5E2)."},

{" file type",          id_,            "->  FILE:",
"The type of references to open I/O streams,\ntypically but not necessarily files.\nThe only literals of this type are stdin, stdout, and stderr."},

#include "table.c"

{0, id_, "->", "->"}
};

#include "builtin.c"
/* clang-format on */

PUBLIC char *opername(int o)
{
    if (o >= 0 && o < (int)(sizeof(optable) / sizeof(optable[0])))
        return optable[o].name;
    return optable[ANON_FUNCT_].name;
}

PUBLIC void (*operproc(int o))(pEnv)
{
    if (o >= 0 && o < (int)(sizeof(optable) / sizeof(optable[0])))
        return optable[o].proc;
    return optable[ANON_FUNCT_].proc;
}

PUBLIC int opertype(int o)
{
    if (o >= 0 && o < (int)(sizeof(optable) / sizeof(optable[0])))
        return o;
    return ILLEGAL_;
}
/* END of INTERP.C */
