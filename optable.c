/*
 *  module  : optable.c
 *  version : 1.15
 *  date    : 12/15/24
 */
#include "globals.h"
#include "runtime.h"
#include "builtin.h"	/* declarations of functions */

/*
 * Specify number of quotations that a combinator consumes.
 */
enum {
    Q0,
    Q1,
    Q2,
    Q3,
    Q4
};

static struct {
    unsigned char qcode, flags;
    char *name;
    void (*proc)(pEnv env);
    char *messg1, *messg2;
} optable[] = {
    /* THESE MUST BE DEFINED IN THE ORDER OF THEIR VALUES */
{Q0,	OK,	"__ILLEGAL",		id_,	"->",
"internal error, cannot happen - supposedly."},

{Q0,	OK,	"__COPIED",		id_,	"->",
"no message ever, used for gc."},

{Q0,	OK,	"__USR",		id_,	"->",
"user node."},

{Q0,	OK,	"__ANON_FUNCT",		id_,	"->",
"op for anonymous function call."},

/* LITERALS */

{Q0,	OK,	" truth value type",	id_,	"->  B",
"The logical type, or the type of truth values.\nIt has just two literals: true and false."},

{Q0,	OK,	" character type",	id_,	"->  C",
"The type of characters. Literals are written with a single quote.\nExamples:  'A  '7  ';  and so on. Unix style escapes are allowed."},

{Q0,	OK,	" integer type",	id_,	"->  I",
"The type of negative, zero or positive integers.\nLiterals are written in decimal notation. Examples:  -123   0   42."},

{Q0,	OK,	" set type",		id_,	"->  {...}",
"The type of sets of small non-negative integers.\nThe maximum is platform dependent, typically the range is 0..31.\nLiterals are written inside curly braces.\nExamples:  {}  {0}  {1 3 5}  {19 18 17}."},

{Q0,	OK,	" string type",		id_,	"->  \"...\"",
"The type of strings of characters. Literals are written inside double quotes.\nExamples: \"\"  \"A\"  \"hello world\" \"123\".\nUnix style escapes are accepted."},

{Q0,	OK,	" list type",		id_,	"->  [...]",
"The type of lists of values of any type (including lists),\nor the type of quoted programs which may contain operators or combinators.\nLiterals of this type are written inside square brackets.\nExamples: []  [3 512 -7]  [john mary]  ['A 'C ['B]]  [dup *]."},

{Q0,	OK,	" float type",		id_,	"->  F",
"The type of floating-point numbers.\nLiterals of this type are written with embedded decimal points (like 1.2)\nand optional exponent specifiers (like 1.5E2)."},

{Q0,	OK,	" file type",		id_,	"->  FILE:",
"The type of references to open I/O streams,\ntypically but not necessarily files.\nThe only literals of this type are stdin, stdout, and stderr."},

{Q0,	OK,	" bignum type",		id_,	"->  F",
"The type of arbitrary precision floating-point numbers.\nLiterals of this type are written with embedded decimal points (like 1.2)\nand optional exponent specifiers (like 1.5E2)."},

#include "table.c"	/* the rest of optable */
};

#include "builtin.c"	/* the primitive functions themselves */

/*
 * tablesize - return the size of the table, to be used when searching from the
 *	       end of the table to the start.
 */
#if defined(BYTECODE) || defined(COMPILER)
int tablesize(void)
{
    return sizeof(optable) / sizeof(optable[0]);
}
#endif

/*
 * nickname - return the name of an operator. If the operator starts with a
 *	      character that is not part of an identifier, then the nick name
 *	      is the part of the string after the first \0.
 */
char *nickname(int ch)
{
    int j;
    char *str;

    j = sizeof(optable) / sizeof(optable[0]);
    if (ch < 0 || ch >= j)
	ch = 0;		/* LCOV_EXCL_LINE */
    str = optable[ch].name;
    if ((ch = *str) == '_' || isalpha(ch))
	return str;
    if (ch != '#')
	while (*str)
	    str++;
    return str + 1;
}

/*
 * opername - return the name of an operator.
 */
char *opername(int ch)
{
    int j;

    j = sizeof(optable) / sizeof(optable[0]);
    if (ch < 0 || ch >= j)
	ch = 0;		/* LCOV_EXCL_LINE */
    return optable[ch].name;
}

/*
 * operindex - return the optable entry for an operator.
 */
int operindex(pEnv env, proc_t proc)
{
    khint_t key;

#ifdef USE_KHASHL
    if ((key = funtab_get(env->prim, (uint64_t)proc)) != kh_end(env->prim))
#else
    if ((key = kh_get(Funtab, env->prim, (uint64_t)proc)) != kh_end(env->prim))
#endif
	return kh_val(env->prim, key);
    return 0;	/* if not found, return 0 */
}

/*
 * add a symbol to the hash table.
 */
void addsymbol(pEnv env, Entry ent, int index)
{
    int rv;
    khint_t key;

#ifdef USE_KHASHL
    key = symtab_put(env->hash, ent.name, &rv);
#else
    key = kh_put(Symtab, env->hash, ent.name, &rv);
#endif
    kh_val(env->hash, key) = index;
}

/*
 * Initialise the symbol table with builtins. There is no need to classify
 * builtins. The hash table contains an index into the symbol table.
 */
void inisymboltable(pEnv env)	/* initialise */
{
    Entry ent;
    khint_t key;
    int i, j, rv;

#ifdef USE_KHASHL
    env->hash = symtab_init();
    env->prim = funtab_init();
#else
    env->hash = kh_init(Symtab);
    env->prim = kh_init(Funtab);
#endif
    j = sizeof(optable) / sizeof(optable[0]);
    for (i = 0; i < j; i++) {
	memset(&ent, 0, sizeof(ent));
	ent.name = optable[i].name;
	ent.flags = optable[i].flags;
	ent.u.proc = optable[i].proc;
	/*
	 * The qcode is copied to the symbol table, telling how many quotations
	 * are consumed by a combinator. The symbols Q0 .. Q4 are translated to
	 * numeric values.
	 */
	ent.qcode = optable[i].qcode;
	/*
	 * If a builtin has received an annotation, then no compile time
	 * evaluation is possible; the builtin should be passed as such
	 * to the compiled program.
	 */
	ent.nofun = *optable[i].messg2 == '[';
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
	addsymbol(env, ent, i);
#ifdef USE_KHASHL
	key = funtab_put(env->prim, (uint64_t)ent.u.proc, &rv);
#else
	key = kh_put(Funtab, env->prim, (uint64_t)ent.u.proc, &rv);
#endif
	kh_val(env->prim, key) = i;
	vec_push(env->symtab, ent);
    }
}
