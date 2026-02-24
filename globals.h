/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.135
 *  date    : 02/23/26
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
#include <assert.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

/*
 * A call to system() is not allowed in WINDOWS_S. It is a kind of security
 * leak and can be disabled here.
 */
#define ALLOW_SYSTEM_CALLS

/*
 * Certain compilers are likely to compile for the Windows platform and that
 * means that WINDOWS can be set. Other compilers need to set this explicitly,
 * if so desired.
 */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__TINYC__)
#define WINDOWS
#endif

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>		/* pollute name space as much as possible */
#include <io.h>			/* also import deprecated POSIX names */
#ifdef __TINYC__
#define strtoll _strtoi64	/* tcc 0.9.27 lacks strtoll */
#endif
#ifdef _MSC_VER
#pragma warning(disable: 4244 4267 4996)
#define kh_packed		/* forget about __attribute__ ((packed)) */
#endif
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

/*
 * The path to the gc-header must be set with -I on the command line.
 */
#include <gc.h>
#include "kvec.h"
#include "khashl.h"

#ifdef NOBDW
#define nodetype(n)	env->memory[n].op
#define nodeleng(n)	env->memory[n].len
#define nodevalue(n)	env->memory[n].u
#define nextnode1(n)	env->memory[n].next
#define nextnode2(n)	env->memory[nextnode1(n)].next
#define nextnode3(n)	env->memory[nextnode2(n)].next
#define nextnode4(n)	env->memory[nextnode3(n)].next
#define nextnode5(n)	env->memory[nextnode4(n)].next
#else
#define nodetype(p)	(p)->op
#define nodevalue(p)	(p)->u
#define nextnode1(p)	(p)->next
#define nextnode2(p)	(nextnode1(p))->next
#define nextnode3(p)	(nextnode2(p))->next
#define nextnode4(p)	(nextnode3(p))->next
#define nextnode5(p)	(nextnode4(p))->next
#endif

#include "macros.h"

/* settings for cflags */
#define IS_ACTIVE	1	/* prevent recursion */
#define IS_USED		2	/* multiple inlining */
#define IS_PRINTED	4	/* print of contents */

/* configure			*/
#define SHELLESCAPE	'$'
#define INPSTACKMAX	10
#define INPLINEMAX	255
#define BUFFERMAX	100	/* smaller buffer */
#define HELPLINEMAX	72
#define MAXNUM		60	/* even smaller buffer */
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

#ifdef COMPILER
    LIST_PRIME_,
#endif

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

typedef struct Env *pEnv;

typedef void (*proc_t)(pEnv);	/* procedure */

#ifdef NOBDW
typedef unsigned Index;
#else
typedef struct Node *Index;
#endif

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
    unsigned char is_user, flags, is_ok, is_root, qcode, nofun, cflags;
    union {
	Index body;
	proc_t proc;
    } u;
} Entry;

KHASHL_MAP_INIT(KH_LOCAL, symtab_t, symtab, const char *, int, kh_hash_str,
		kh_eq_str)
KHASHL_MAP_INIT(KH_LOCAL, funtab_t, funtab, uint64_t, int, kh_hash_uint64,
		kh_eq_generic)

/*
 * Anonymous vectors are now a thing of the past. They cannot be passed as a
 * parameter and although such usage is not required, it is better to use named
 * vectors going forward.
 */
typedef char *char_p;		/* pointer to char */
typedef char char_s;		/* dynamic string */
typedef char char_b;		/* push back buffer */
typedef struct {
    Index from, *to;
} pair_t;

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
    char *filename;		/* first include file */
    char *homedir;		/* HOME or HOMEPATH */
    char *mod_name;		/* name of module */
    vector(char_p) *pathnames;	/* pathnames to be searched when including */
    vector(char_s) *string;	/* value */
    vector(char_b) *pushback;	/* push back buffer */
    vector(Token) *tokens;	/* read ahead table */
    vector(Entry) *symtab;	/* symbol table */
    symtab_t *hash;		/* hash tables that index the symbol table */
    funtab_t *prim;
    Types bucket;		/* used by NEWNODE defines */
#ifdef NOBDW
    clock_t gc_clock;
    vector(pair_t) *gc_stack;	/* garbage collection w/o recursion, sort of */
    Node *memory;		/* dynamic memory */
    Node *old_memory;
    size_t memorymax;
    Index memoryindex, mem_low;
    Index dump, dump1, dump2, dump3, dump4, dump5, inits;
#endif
    Index prog, stck, conts;
#ifdef COMPILER
    FILE *declfp, *outfp;
#endif
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
    unsigned char variable_busy;
#ifdef NOBDW
    unsigned char flibrary_busy;
#else
    signed char bytecoding;
    signed char compiling;
#endif
} Env;

typedef struct table_t {
    proc_t proc;
    char *name;
} table_t;

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
void inilinebuffer(pEnv env);
int getch(pEnv env);
void ungetch(int ch);
void error(char *str);
int include(pEnv env, char *name);
int getsym(pEnv env, int ch);
/* utils.c */
Index newnode(pEnv env, Operator o, Types u, Index r);
Index newnode2(pEnv env, Index n, Index r);
void my_memoryindex(pEnv env);
void my_memorymax(pEnv env);
#ifdef NOBDW
void inimem1(pEnv env, int status);
void inimem2(pEnv env);
void printnode(pEnv env, Index p);
void my_gc(pEnv env);
#endif
/* error.c */
void execerror(pEnv env, char *message, char *op);
/* factor.c */
int readfactor(pEnv env, int ch, int *rv);	/* read a JOY factor */
int readterm(pEnv env, int ch);
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
int tablesize(void);
char *nickname(int ch);
char *opername(int o);
int operindex(pEnv env, proc_t proc);
void inisymboltable(pEnv env);			/* initialise */
void addsymbol(pEnv env, Entry ent, int index);
/* print.c */
void print(pEnv env);
/* repl.c */
void repl(pEnv env);
/* setraw.c */
void SetRaw(void);
void SetNormal(void);
/* symbol.c */
int lookup(pEnv env, char *name);
int enteratom(pEnv env, char *name);
int compound_def(pEnv env, int ch);
/* undefs.c */
void hide_inner_modules(pEnv env, int flag);
/* write.c */
void writechar(int ch, char *str);
void writestring(int ch, char *str);
void writefactor(pEnv env, Index n, FILE *fp);
void writeterm(pEnv env, Index n, FILE *fp);
#ifdef BYTECODE
/* bytecode.c */
void bytecode(pEnv env, Node *list);
void initbytes(pEnv env, char *ext);
void exitbytes(pEnv env);
/* compeval.c */
void compeval(pEnv env, FILE *fp);
/* dumpbyte.c */
void dumpbyte(pEnv env, FILE *fp);
/* optimize.c */
void optimize(pEnv env, FILE *fp);
/* renumber.c */
void renumber(pEnv env, FILE *fp);
#endif
#if defined(BYTECODE) || defined(COMPILER)
/* computil.c */
Node *reverse(Node *cur);
char *outputfile(char *inputfile, char *suffix);
#endif
#if defined(BYTECODE) || defined(COMPILER) || defined(RUNBYTES)
/* readbyte.c */
void readbyte(pEnv env, FILE *fp);
#endif
#ifdef COMPILER
/* compiler.c */
void printnode(pEnv env, Node *node);
void printstack(pEnv env);
void compile(pEnv env, Node *node);
void initcompile(pEnv env, int joy);
void exitcompile(pEnv env);
/* identify.c */
const char *identifier(const char *str);
const char *unidentify(const char *str);
/* instance.c */
int instance(pEnv env, char *name, int qcode);
/* outfiles.c */
void initout(void);
FILE *nextfile(void);
void closefile(FILE *fp);
void printout(FILE *fp);
void closeout(void);
/* readtemp.c */
int testtemp(char *file);
void readtemp(pEnv env, char *file, Node *nodes[], int found, int seqnr);
#endif
#endif
