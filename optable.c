/*
 *  module  : optable.c
 *  version : 1.5
 *  date    : 05/27/24
 */
#include "globals.h"

#define NEWNODE(o, u, r)						\
    (env->bucket.lis = newnode(env, o, u, r), env->bucket.lis)
#define USR_NEWNODE(u, r) (env->bucket.ent = u, NEWNODE(USR_, env->bucket, r))
#define ANON_FUNCT_NEWNODE(u, r)					\
    (env->bucket.proc = u, NEWNODE(ANON_FUNCT_, env->bucket, r))
#define BOOLEAN_NEWNODE(u, r)						\
    (env->bucket.num = u, NEWNODE(BOOLEAN_, env->bucket, r))
#define CHAR_NEWNODE(u, r) (env->bucket.num = u, NEWNODE(CHAR_, env->bucket, r))
#define INTEGER_NEWNODE(u, r)						\
    (env->bucket.num = u, NEWNODE(INTEGER_, env->bucket, r))
#define SET_NEWNODE(u, r) (env->bucket.num = u, NEWNODE(SET_, env->bucket, r))
#define STRING_NEWNODE(u, r)						\
    (env->bucket.str = u, NEWNODE(STRING_, env->bucket, r))
#define LIST_NEWNODE(u, r) (env->bucket.lis = u, NEWNODE(LIST_, env->bucket, r))
#define FLOAT_NEWNODE(u, r)						\
    (env->bucket.dbl = u, NEWNODE(FLOAT_, env->bucket, r))
#define FILE_NEWNODE(u, r) (env->bucket.fil = u, NEWNODE(FILE_, env->bucket, r))

#define FLOATABLE							\
    (nodetype(env->stck) == INTEGER_ || nodetype(env->stck) == FLOAT_)
#define FLOATABLE2							\
    ((nodetype(env->stck) == FLOAT_					\
    && nodetype(nextnode1(env->stck)) == FLOAT_)			\
    || (nodetype(env->stck) == FLOAT_					\
    && nodetype(nextnode1(env->stck)) == INTEGER_)			\
    || (nodetype(env->stck) == INTEGER_					\
    && nodetype(nextnode1(env->stck)) == FLOAT_))
#define FLOATVAL							\
    (nodetype(env->stck) == FLOAT_					\
    ? nodevalue(env->stck).dbl						\
    : (double)nodevalue(env->stck).num)
#define FLOATVAL2							\
    (nodetype(nextnode1(env->stck)) == FLOAT_				\
    ? nodevalue(nextnode1(env->stck)).dbl				\
    : (double)nodevalue(nextnode1(env->stck)).num)
#define FLOAT_U(OPER)							\
    if (FLOATABLE)							\
    { UNARY(FLOAT_NEWNODE, OPER(FLOATVAL)); return; }
#define FLOAT_P(OPER)							\
    if (FLOATABLE2)							\
    { BINARY(FLOAT_NEWNODE, OPER(FLOATVAL2, FLOATVAL)); return; }
#define FLOAT_I(OPER)							\
    if (FLOATABLE2)							\
    { BINARY(FLOAT_NEWNODE, (FLOATVAL2)OPER(FLOATVAL)); return; }

#ifndef NCHECK
#define ONEPARAM(NAME)							\
    if (!env->stck)							\
    execerror("one parameter", NAME)
#define TWOPARAMS(NAME)							\
    if (!env->stck || !nextnode1(env->stck))				\
    execerror("two parameters", NAME)
#define THREEPARAMS(NAME)						\
    if (!env->stck || !nextnode1(env->stck) || !nextnode2(env->stck))	\
    execerror("three parameters", NAME)
#define FOURPARAMS(NAME)						\
    if (!env->stck || !nextnode1(env->stck)				\
    || !nextnode2(env->stck) || !nextnode3(env->stck))			\
    execerror("four parameters", NAME)
#define FIVEPARAMS(NAME)						\
    if (!env->stck || !nextnode1(env->stck) || !nextnode2(env->stck)	\
    || !nextnode3(env->stck) || !nextnode4(env->stck))			\
    execerror("five parameters", NAME)
#define ONEQUOTE(NAME)							\
    if (nodetype(env->stck) != LIST_)					\
    execerror("quotation as top parameter", NAME)
#define TWOQUOTES(NAME)							\
    ONEQUOTE(NAME);							\
    if (nodetype(nextnode1(env->stck)) != LIST_)			\
    execerror("quotation as second parameter", NAME)
#define THREEQUOTES(NAME)						\
    TWOQUOTES(NAME);							\
    if (nodetype(nextnode2(env->stck)) != LIST_)			\
    execerror("quotation as third parameter", NAME)
#define FOURQUOTES(NAME)						\
    THREEQUOTES(NAME);							\
    if (nodetype(nextnode3(env->stck)) != LIST_)			\
    execerror("quotation as fourth parameter", NAME)
#define SAME2TYPES(NAME)						\
    if (nodetype(env->stck) != nodetype(nextnode1(env->stck)))		\
    execerror("two parameters of the same type", NAME)
#define STRING(NAME)							\
    if (nodetype(env->stck) != STRING_)					\
    execerror("string", NAME)
#define STRING2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != STRING_)			\
    execerror("string as second parameter", NAME)
#define INTEGER(NAME)							\
    if (nodetype(env->stck) != INTEGER_)				\
    execerror("integer", NAME)
#define POSITIVEINTEGER(NAME)						\
    if (nodetype(env->stck) != INTEGER_ || nodevalue(env->stck).num < 0)\
    execerror("non-negative integer", NAME)
#define INTEGER2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != INTEGER_)			\
    execerror("integer as second parameter", NAME)
#define CHARACTER(NAME)							\
    if (nodetype(env->stck) != CHAR_)					\
    execerror("character", NAME)
#define INTEGERS2(NAME)							\
    if (nodetype(env->stck) != INTEGER_					\
    || nodetype(nextnode1(env->stck)) != INTEGER_)			\
    execerror("two integers", NAME)
#define NUMERICTYPE(NAME)						\
    if (nodetype(env->stck) != INTEGER_ && nodetype(env->stck) != CHAR_	\
    && nodetype(env->stck) != BOOLEAN_)					\
    execerror("numeric", NAME)
#define NUMERIC2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != INTEGER_			\
    && nodetype(nextnode1(env->stck)) != CHAR_)				\
    execerror("numeric second parameter", NAME)
#define CHECKNUMERIC(NODE, NAME)					\
    if (nodetype(NODE) != INTEGER_)					\
    execerror("numeric list", NAME)
#define FLOAT(NAME)							\
    if (!FLOATABLE)							\
    execerror("float or integer", NAME);
#define FLOAT2(NAME)							\
    if (!(FLOATABLE2							\
    || (nodetype(env->stck) == INTEGER_					\
    && nodetype(nextnode1(env->stck)) == INTEGER_)))			\
    execerror("two floats or integers", NAME)
#define FILE(NAME)							\
    if (nodetype(env->stck) != FILE_ || !nodevalue(env->stck).fil)	\
    execerror("file", NAME)
#define CHECKZERO(NAME)							\
    if (nodevalue(env->stck).num == 0)					\
    execerror("non-zero operand", NAME)
#define CHECKDIVISOR(NAME)						\
    if ((nodetype(env->stck) == FLOAT_ && nodevalue(env->stck).dbl == 0.0) \
    || (nodetype(env->stck) == INTEGER_ && nodevalue(env->stck).num == 0)) \
    execerror("non-zero divisor", "divide");
#define LIST(NAME)							\
    if (nodetype(env->stck) != LIST_)					\
    execerror("list", NAME)
#define LIST2(NAME)							\
    if (nodetype(nextnode1(env->stck)) != LIST_)			\
    execerror("list as second parameter", NAME)
#define USERDEF(NAME)							\
    if (nodetype(env->stck) != USR_)					\
    execerror("user defined symbol", NAME)
#define USERDEF2(NODE, NAME)						\
    if (nodetype(NODE) != USR_)						\
    execerror("user defined symbol", NAME)
#define CHECKLIST(OPR, NAME)						\
    if (OPR != LIST_)							\
    execerror("internal list", NAME)
#define CHECKSETMEMBER(NODE, NAME)					\
    if ((nodetype(NODE) != INTEGER_ && nodetype(NODE) != CHAR_)		\
    || nodevalue(NODE).num < 0 || nodevalue(NODE).num >= SETSIZE)	\
    execerror("small numeric", NAME)
#define CHECKCHARACTER(NODE, NAME)					\
    if (nodetype(NODE) != CHAR_)					\
    execerror("character", NAME)
#define CHECKEMPTYSET(SET, NAME)					\
    if (SET == 0)							\
    execerror("non-empty set", NAME)
#define CHECKEMPTYSTRING(STRING, NAME)					\
    if (*STRING == '\0')						\
    execerror("non-empty string", NAME)
#define CHECKEMPTYLIST(LIST, NAME)					\
    if (!LIST)								\
    execerror("non-empty list", NAME)
#define CHECKSTACK(NAME)						\
    if (!env->stck)							\
    execerror("non-empty stack", NAME)
#define CHECKVALUE(NAME)						\
    if (!env->stck)							\
    execerror("value to push", NAME)
#define CHECKNAME(STRING, NAME)						\
    if (!STRING || *STRING)						\
    execerror("valid name", NAME)
#define CHECKFORMAT(SPEC, NAME)						\
    if (!strchr("dioxX", SPEC))						\
    execerror("one of: d i o x X", NAME)
#define CHECKFORMATF(SPEC, NAME)					\
    if (!strchr("eEfgG", SPEC))						\
    execerror("one of: e E f g G", NAME)
#define POSITIVEINDEX(INDEX, NAME)					\
    if ((nodetype(INDEX) != INTEGER_ &&	nodetype(INDEX) != BOOLEAN_) || \
    nodevalue(INDEX).num < 0) execerror("non-negative integer", NAME)
#define INDEXTOOLARGE(NAME) execerror("smaller index", NAME)
#define BADAGGREGATE(NAME) execerror("aggregate parameter", NAME)
#define BADDATA(NAME) execerror("different type", NAME)
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
#define POSITIVEINTEGER(NAME)
#define INTEGER2(NAME)
#define CHARACTER(NAME)
#define INTEGERS2(NAME)
#define NUMERICTYPE(NAME)
#define NUMERIC2(NAME)
#define CHECKNUMERIC(NODE, NAME)
#define FLOAT(NAME)
#define FLOAT2(NAME)
#define FILE(NAME)
#define CHECKZERO(NAME)
#define CHECKDIVISOR(NAME)
#define LIST(NAME)
#define LIST2(NAME)
#define USERDEF(NAME)
#define USERDEF2(NODE, NAME)
#define CHECKLIST(OPR, NAME)
#define CHECKSETMEMBER(NODE, NAME)
#define CHECKCHARACTER(NODE, NAME)
#define CHECKEMPTYSET(SET, NAME)
#define CHECKEMPTYSTRING(STRING, NAME)
#define CHECKEMPTYLIST(LIST, NAME)
#define CHECKSTACK(NAME)
#define CHECKVALUE(NAME)
#define CHECKNAME(STRING, NAME)
#define CHECKFORMAT(SPEC, NAME)
#define CHECKFORMATF(SPEC, NAME)
#define POSITIVEINDEX(INDEX, NAME)
#define INDEXTOOLARGE(NAME)
#define BADAGGREGATE(NAME)
#define BADDATA(NAME)
#endif

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
#define UNARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, nextnode1(env->stck))
#define BINARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, nextnode2(env->stck))
#define GNULLARY(TYPE, VALUE) env->stck = newnode(env, TYPE, (VALUE), env->stck)
#define GUNARY(TYPE, VALUE)						\
    env->stck = newnode(env, TYPE, (VALUE), nextnode1(env->stck))
#define GBINARY(TYPE, VALUE)						\
    env->stck = newnode(env, TYPE, (VALUE), nextnode2(env->stck))
#define GTERNARY(TYPE, VALUE)						\
    env->stck = newnode(env, TYPE, (VALUE), nextnode3(env->stck))

#include "src/andorxor.h"
#include "src/bfloat.h"
#include "src/comprel.h"
#include "src/comprel2.h"
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
#include "builtin.h"	/* declarations of functions */

static struct {
    char flags;
    char *name;
    void (*proc)(pEnv env);
    char *messg1, *messg2;
} optable[] = {
    /* THESE MUST BE DEFINED IN THE ORDER OF THEIR VALUES */
{OK,	"__ILLEGAL",		id_,	    "->",
"internal error, cannot happen - supposedly."},

{OK,	"__COPIED",		id_,	    "->",
"no message ever, used for gc."},

{OK,	"__USR",		id_,	    "->",
"user node."},

{OK,	"__ANON_FUNCT",		id_,	    "->",
"op for anonymous function call."},

/* LITERALS */

{OK,	" truth value type",	id_,	    "->  B",
"The logical type, or the type of truth values.\nIt has just two literals: true and false."},

{OK,	" character type",	id_,	    "->  C",
"The type of characters. Literals are written with a single quote.\nExamples:  'A  '7  ';  and so on. Unix style escapes are allowed."},

{OK,	" integer type",	id_,	    "->  I",
"The type of negative, zero or positive integers.\nLiterals are written in decimal notation. Examples:  -123   0   42."},

{OK,	" set type",		id_,	    "->  {...}",
"The type of sets of small non-negative integers.\nThe maximum is platform dependent, typically the range is 0..31.\nLiterals are written inside curly braces.\nExamples:  {}  {0}  {1 3 5}  {19 18 17}."},

{OK,	" string type",		id_,	    "->  \"...\"",
"The type of strings of characters. Literals are written inside double quotes.\nExamples: \"\"  \"A\"  \"hello world\" \"123\".\nUnix style escapes are accepted."},

{OK,	" list type",		id_,	    "->  [...]",
"The type of lists of values of any type (including lists),\nor the type of quoted programs which may contain operators or combinators.\nLiterals of this type are written inside square brackets.\nExamples: []  [3 512 -7]  [john mary]  ['A 'C ['B]]  [dup *]."},

{OK,	" float type",		id_,	    "->  F",
"The type of floating-point numbers.\nLiterals of this type are written with embedded decimal points (like 1.2)\nand optional exponent specifiers (like 1.5E2)."},

{OK,	" file type",		id_,	    "->  FILE:",
"The type of references to open I/O streams,\ntypically but not necessarily files.\nThe only literals of this type are stdin, stdout, and stderr."},

{OK,	" bignum type",		id_,	    "->  F",
"The type of arbitrary precision floating-point numbers.\nLiterals of this type are written with embedded decimal points (like 1.2)\nand optional exponent specifiers (like 1.5E2)."},

#include "table.c"	/* the rest of optable */
};

#include "builtin.c"	/* the primitive functions themselves */

/*
 * nickname - return the name of an operator. If the operator starts with a
 *	      character that is not part of an identifier, then the nick name
 *	      is the part of the string after the first \0.
 */
char *nickname(int ch)
{
    char *str;

    str = optable[ch].name;
    if ((ch = *str) == '_' || isalpha(ch))
	return str;
    while (*str)
	str++;
    return str + 1;
}

/*
 * opername - return the name of an operator, or 0 at end of optable.
 */
char *opername(int o)
{
    int size;

    size = sizeof(optable) / sizeof(optable[0]);
    return o >= 0 && o < size ? optable[o].name : 0;
}

/*
 * operindex - return the optable entry for an operator.
 */
int operindex(pEnv env, proc_t proc)
{
    khint_t key;

    if ((key = funtab_get(env->prim, (uint64_t)proc)) != kh_end(env->prim))
	return kh_val(env->prim, key);
    return ANON_FUNCT_;	/* if not found, return the index of ANON_FUNCT_ */
}

/*
 * Initialise the symbol table with builtins. There is no need to classify
 * builtins. The hash table contains an index into the symbol table.
 */
void inisymboltable(pEnv env) /* initialise */
{
    Entry ent;
    khint_t key;
    int i, j, rv;

    env->hash = symtab_init();
    env->prim = funtab_init();
    j = sizeof(optable) / sizeof(optable[0]);
    for (i = 0; i < j; i++) {
	ent.name = optable[i].name;
	ent.is_user = 0;
	ent.flags = optable[i].flags;
	ent.u.proc = optable[i].proc;
	if (env->ignore)
	    switch (ent.flags) {
	    case IGNORE_OK:
		ent.u.proc = id_;
		break;
	    case IGNORE_POP:
		ent.u.proc = pop_;
		break;
	    case POSTPONE:
	    case IGNORE_PUSH:
		ent.u.proc = __dump_;
		break;
	    }
	key = symtab_put(env->hash, ent.name, &rv);
	kh_val(env->hash, key) = i;
	key = funtab_put(env->prim, (uint64_t)ent.u.proc, &rv);
	kh_val(env->prim, key) = i;
	vec_push(env->symtab, ent);
    }
}
