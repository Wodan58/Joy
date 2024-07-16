/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.103
 *  date    : 07/01/24
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>		/* pollute name space as much as possible */
#include <io.h>			/* also import deprecated POSIX names */
#pragma warning(disable: 4244 4267 4996)
#define kh_packed		/* forget about __attribute__ ((packed)) */
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#ifndef NOBDW
#include <gc.h>
#else
#include "gc.h"
#endif
#include "kvec.h"
#include "khashl.h"

#define POP(X) X = nextnode1(X)

#define USR_NEWNODE(u, r)						\
    (env->bucket.ent = u, newnode(env, USR_, env->bucket, r))
#define ANON_FUNCT_NEWNODE(u, r)					\
    (env->bucket.proc = u, newnode(env, ANON_FUNCT_, env->bucket, r))
#define BOOLEAN_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, BOOLEAN_, env->bucket, r))
#define CHAR_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, CHAR_, env->bucket, r))
#define INTEGER_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, INTEGER_, env->bucket, r))
#define SET_NEWNODE(u, r)						\
    (env->bucket.num = u, newnode(env, SET_, env->bucket, r))
#define STRING_NEWNODE(u, r)						\
    (env->bucket.str = u, newnode(env, STRING_, env->bucket, r))
#define LIST_NEWNODE(u, r)						\
    (env->bucket.lis = u, newnode(env, LIST_, env->bucket, r))
#define FLOAT_NEWNODE(u, r)						\
    (env->bucket.dbl = u, newnode(env, FLOAT_, env->bucket, r))
#define FILE_NEWNODE(u, r)						\
    (env->bucket.fil = u, newnode(env, FILE_, env->bucket, r))

#define NULLARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, env->stck)
#define UNARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, nextnode1(env->stck))
#define BINARY(CONSTRUCTOR, VALUE)					\
    env->stck = CONSTRUCTOR(VALUE, nextnode2(env->stck))

#ifdef NOBDW
#define nodetype(n)  env->memory[n].op
#define nodeleng(n)  env->memory[n].len
#define nodevalue(n) env->memory[n].u
#define nextnode1(n) env->memory[n].next
#define nextnode2(n) env->memory[nextnode1(n)].next
#define nextnode3(n) env->memory[nextnode2(n)].next
#define nextnode4(n) env->memory[nextnode3(n)].next
#define nextnode5(n) env->memory[nextnode4(n)].next
#else
#define nodetype(p)  (p)->op
#define nodevalue(p) (p)->u
#define nextnode1(p) (p)->next
#define nextnode2(p) (nextnode1(p))->next
#define nextnode3(p) (nextnode2(p))->next
#define nextnode4(p) (nextnode3(p))->next
#define nextnode5(p) (nextnode4(p))->next
#ifdef TRACEGC
#undef TRACEGC
#endif
#endif

/* configure			*/
#define SHELLESCAPE	'$'
#define INPSTACKMAX	10
#define INPLINEMAX	255
#define BUFFERMAX	80	/* smaller buffer */
#define HELPLINEMAX	72
#define MAXNUM		40	/* even smaller buffer */
#define FILENAMEMAX	14
#define DISPLAYMAX	10	/* nesting in HIDE & MODULE */
#define INIECHOFLAG	0
#define INIAUTOPUT	1
#define INITRACEGC	1
#define INIUNDEFERROR	0
#define INIWARNING	1

/* installation dependent	*/
#define SETSIZE (int)(CHAR_BIT * sizeof(uint64_t))	/* from limits.h */
#define MAXINT_ INT64_MAX				/* from stdint.h */

/* symbols from getsym		*/
enum {
    ILLEGAL_,
    COPIED_,
    USR_,
    ANON_FUNCT_,
    BOOLEAN_,
    CHAR_,
    INTEGER_,
    SET_,
    STRING_,
    LIST_,
    FLOAT_,
    FILE_,
    BIGNUM_,

    LIBRA,
    EQDEF,
    HIDE,
    IN__,
    MODULE_,
    PRIVATE,
    PUBLIC,
    CONST_
};

typedef enum {
    OK,
    IGNORE_OK,
    IGNORE_PUSH,
    IGNORE_POP,
    IMMEDIATE,
    POSTPONE
} Flags;

typedef enum {
    ABORT_NONE,
    ABORT_RETRY,
    ABORT_QUIT
} Abort;

/* types			*/
typedef unsigned char Operator;	/* opcode / datatype */

#ifdef NOBDW
typedef unsigned Index;
#else
typedef struct Node *Index;
#endif

typedef struct Env *pEnv;

typedef void (*proc_t)(pEnv);	/* procedure */

typedef union {
    int64_t num;	/* USR, BOOLEAN, CHAR, INTEGER */
    proc_t proc;	/* ANON_FUNCT */
    uint64_t set;	/* SET */
    char *str;		/* STRING */
    Index lis;		/* LIST */
    double dbl;		/* FLOAT */
    FILE *fil;		/* FILE */
    int ent;		/* SYMBOL */
} Types;

#ifdef NOBDW
typedef struct Node {
    unsigned op :  4,
	    len : 28;	/* length of string */
    Index next;
    Types u;
} Node;
#else
typedef struct Node {
    Operator op;
    Index next;
    Types u;
} Node;
#endif

typedef struct Token {
    Operator op;
    int x, y, pos;
    Types u;
} Token;

typedef struct Entry {
    char *name;
    unsigned char is_user, flags, is_ok, is_root, is_last;
    union {
	Index body;
	proc_t proc;
    } u;
} Entry;

KHASHL_MAP_INIT(KH_LOCAL, symtab_t, symtab, const char *, int, kh_hash_str,
		kh_eq_str)
KHASHL_MAP_INIT(KH_LOCAL, funtab_t, funtab, uint64_t, int, kh_hash_uint64,
		kh_eq_generic)

typedef struct Env {
    jmp_buf finclude;		/* return point in finclude */
    double nodes;		/* statistics */
    double avail;
    double collect;
    double calls;
    double opers;
    double dbl;			/* numerics */
    int64_t num;
    char *str;			/* string */
    clock_t startclock;		/* main */
    char **g_argv;		/* command line */
    char *homedir;		/* HOME or USERPROFILE */
    char *pathname;		/* pathname of joy binary */
    char *mod_name;		/* name of module */
    vector(char) *string;	/* value */
    vector(char) *pushback;	/* push back buffer */
    vector(Token) *tokens;	/* read ahead table */
    vector(Entry) *symtab;	/* symbol table */
    symtab_t *hash;		/* hash tables that index the symbol table */
    funtab_t *prim;
    Types bucket;		/* used by NEWNODE defines */
#ifdef NOBDW
    clock_t gc_clock;
    Node *memory;		/* dynamic memory */
    Index conts, dump, dump1, dump2, dump3, dump4, dump5;
#endif
    Index prog, stck;
    int g_argc;			/* command line */
    int hide_stack[DISPLAYMAX];
    struct {
	char *name;
	int hide;
    } module_stack[DISPLAYMAX];
    Operator sym;		/* symbol */
    unsigned char inlining;
    unsigned char autoput;
    unsigned char autoput_set;
    unsigned char echoflag;
    unsigned char tracegc;
    unsigned char undeferror;
    unsigned char undeferror_set;
    unsigned char debugging;
    unsigned char ignore;
    unsigned char overwrite;
    unsigned char printing;
    unsigned char finclude_busy;
    unsigned char flibrary_busy;
    unsigned char variable_busy;
} Env;

/* GOOD REFS:
	005.133l H4732		A LISP interpreter in C
	Manna p139  recursive Ackermann SCHEMA

   OTHER DATA TYPES
	WORD = "ABCD" - up to four chars
	LIST of SETs of char [S0 S1 S2 S3]
		LISTS - binary tree [left right]
			" with info [info left right] "
	STRING of 32 chars = 32 * 8 bits = 256 bits = bigset
	CHAR = 2 HEX
	32 SET = 2 * 16SET
*/

/* Public procedures: */
/* main.c */
void abortexecution_(int num);
void fatal(char *str);
/* interp.c */
void exeterm(pEnv env, Index n);
/* scan.c */
void inilinebuffer(FILE *fp, char *str);
int getch(pEnv env);
void ungetch(int ch);
void error(char *str);
void execerror(char *message, char *op);
int include(pEnv env, char *name);
int getsym(pEnv env, int ch);
/* utils.c */
Index newnode(pEnv env, Operator o, Types u, Index r);
Index newnode2(pEnv env, Index n, Index r);
void my_memoryindex(pEnv env);
void my_memorymax(pEnv env);
#ifdef NOBDW
void writedump(pEnv env, Index n, FILE *fp);
void inimem1(pEnv env, int status);
void inimem2(pEnv env);
void printnode(pEnv env, Index p);
void my_gc(pEnv env);
void *check_malloc(size_t leng);
void *check_realloc(void *ptr, size_t leng);
void *check_strdup(char *ptr);
#endif
/* factor.c */
int readfactor(pEnv env, int ch, int *rv);	/* read a JOY factor */
int readterm(pEnv env, int ch);
void writefactor(pEnv env, Index n, FILE *fp);
void writeterm(pEnv env, Index n, FILE *fp);
void writedump(pEnv env, Index n, FILE *fp);
/* module.c */
void savemod(int *hide, int *modl, int *hcnt);
void undomod(int hide, int modl, int hcnt);
void initmod(pEnv env, char *name);
void initpriv(pEnv env);
void stoppriv(void);
void exitpriv(void);
void exitmod(void);
char *classify(pEnv env, char *name);
int qualify(pEnv env, char *name);
/* optable.c */
char *nickname(int ch);
char *opername(int o);
int operindex(pEnv env, proc_t proc);
void inisymboltable(pEnv env);			/* initialise */
/* symbol.c */
int lookup(pEnv env, char *name);
int enteratom(pEnv env, char *name);
int compound_def(pEnv env, int ch);
/* undefs.c */
void hide_inner_modules(pEnv env, int flag);
#endif
