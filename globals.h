/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.93
 *  date    : 03/24/24
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
#ifndef NOBDW
#include <gc.h>
#else
#include "gc.h"
#endif
#include "kvec.h"
#include "khash.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#pragma warning(disable: 4244 4267 4996)
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#ifdef NOBDW
#define nodetype(n)  vec_at(env->memory, n).op
#define nodevalue(n) vec_at(env->memory, n).u
#define nextnode1(n) vec_at(env->memory, n).next
#define nextnode2(n) vec_at(env->memory, nextnode1(n)).next
#define nextnode3(n) vec_at(env->memory, nextnode2(n)).next
#define nextnode4(n) vec_at(env->memory, nextnode3(n)).next
#define nextnode5(n) vec_at(env->memory, nextnode4(n)).next
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
#define MAXNUM		32	/* even smaller buffer */
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

typedef struct Node {
    Types u;
    Operator op;
    Index next;
} Node;

typedef struct Entry {
    char *name, is_user, flags;
    union {
	Index body;
	proc_t proc;
    } u;
} Entry;

KHASH_MAP_INIT_STR(Symtab, int)
KHASH_MAP_INIT_INT64(Funtab, int)

typedef struct Env {
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
    char *pathname;
    char *mod_name;		/* name of module */
    vector(char) *string;	/* value */
    vector(char) *pushback;	/* push back buffer */
    vector(Node) *tokens;	/* read ahead table */
    vector(Entry) *symtab;	/* symbol table */
    khash_t(Symtab) *hash;
    khash_t(Funtab) *prim;
    Types bucket;		/* used by NEWNODE defines */
#ifdef NOBDW
    clock_t gc_clock;
    vector(Node) *memory;	/* dynamic memory */
    Index prog, stck, conts, dump, dump1, dump2, dump3, dump4, dump5;
#else
    Node *prog, *stck;
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
    unsigned char echoflag_set;
    unsigned char tracegc;
    unsigned char undeferror;
    unsigned char undeferror_set;
    unsigned char debugging;
    unsigned char ignore;
    unsigned char overwrite;
    unsigned char printing;
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
/* factor.c */
int readfactor(pEnv env, int ch, int *rv);	/* read a JOY factor */
int readterm(pEnv env, int ch);
void writefactor(pEnv env, Index n, FILE *fp);
void writeterm(pEnv env, Index n, FILE *fp);
#ifdef NOBDW
void writedump(pEnv env, Index n, FILE *fp);
#endif
/* interp.c */
void exeterm(pEnv env, Index n);
/* main.c */
void abortexecution_(int num);
void stats(pEnv env);
void dump(pEnv env);
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
void inisymboltable(pEnv env);	/* initialise */
/* scan.c */
void inilinebuffer(FILE *fp, char *str);
int getch(pEnv env);
void ungetch(int ch);
void error(char *message);
void execerror(char *str, char *op);
void include(pEnv env, char *name);
int getsym(pEnv env, int ch);
/* symbol.c */
int lookup(pEnv env, char *name);
int enteratom(pEnv env, char *name);
int compound_def(pEnv env, int ch);
/* undefs.c */
void hide_inner_modules(pEnv env, int flag);
/* utils.c */
#ifdef NOBDW
void inimem1(pEnv env, int status);
void inimem2(pEnv env);
void printnode(pEnv env, Index p);
void my_gc(pEnv env);
#endif
Index newnode(pEnv env, Operator o, Types u, Index r);
void my_memoryindex(pEnv env);
void my_memorymax(pEnv env);
#endif
