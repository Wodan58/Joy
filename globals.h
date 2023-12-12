/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.81
 *  date    : 12/12/23
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
#include <gc.h>
#include "kvec.h"
#include "khash.h"

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
#endif

/* configure			*/
#define SHELLESCAPE '$'
#define INPSTACKMAX 10
#define INPLINEMAX 255
#define BUFFERMAX 80
#define ALEN 45 /* module + member */
#define DISPLAYMAX 10 /* nesting in HIDE & MODULE */
#define INIECHOFLAG 0
#define INIAUTOPUT 1
#define INITRACEGC 1
#define INIUNDEFERROR 0

/* installation dependent	*/
#define SETSIZE (int)(CHAR_BIT * sizeof(uint64_t))	/* from limits.h */
#define MAXINT INT64_MAX				/* from stdint.h */

/* symbols from getsym		*/
#define ILLEGAL_ 0
#define COPIED_ 1
#define KEYWORD_ 1
#define USR_ 2
#define ANON_FUNCT_ 3
#define BOOLEAN_ 4
#define CHAR_ 5
#define INTEGER_ 6
#define SET_ 7
#define STRING_ 8
#define LIST_ 9
#define FLOAT_ 10
#define FILE_ 11
#define LBRACK 900
#define LBRACE 901
#define LPAREN 902
#define ATOM 999 /* last legal factor begin */
#define RBRACK 1001
#define RPAREN 1003
#define RBRACE 1005
#define PERIOD 1006
#define SEMICOL 1007
#define LIBRA 1100
#define EQDEF 1101
#define HIDE 1102
#define IN 1103
#define END 1104
#define MODULE 1105
#define JPRIVATE 1106
#define JPUBLIC 1107

#define PRIVATE
#define PUBLIC

typedef enum {
    NOT_USED,
    MY_ABORT,
    MOD_NAME,
    EXEC_ERR
} Aborts;

typedef enum {
    OK,
    IMMEDIATE
} Flags;

/* types			*/
typedef int Symbol;		/* symbol created by scanner */
typedef int Operator;		/* opcode / datatype */

#ifdef NOBDW
typedef unsigned Index;
#else
typedef struct Node *Index;
#endif

typedef unsigned pEntry;	/* index in symbol table */

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
    pEntry ent;		/* SYMBOL */
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

typedef struct Token {
    Types yylval;
    Symbol symb;
} Token;

KHASH_MAP_INIT_STR(Symtab, pEntry)

typedef struct Env {
    vector(Token) *tokens; /* read ahead table */
    vector(Entry) *symtab; /* symbol table */
    khash_t(Symtab) *hash;
#ifdef NOBDW
    clock_t gc_clock;
    vector(Node) *memory;  /* dynamic memory */
    Index prog, stck, conts, dump, dump1, dump2, dump3, dump4, dump5;
#else
    Node *prog, *stck;
#endif
    Types yylval, bucket;  /* used by NEWNODE defines */
    clock_t startclock;    /* main */
    FILE *srcfile;
    char *pathname;
    char **g_argv;
    int g_argc;
    pEntry location;       /* getsym */
    Symbol symb;	   /* scanner */
    int hide_stack[DISPLAYMAX];
    struct module {
	char *name;
	int hide;
    } module_stack[DISPLAYMAX];
    unsigned char autoput; /* options */
    unsigned char echoflag;
    unsigned char undeferror;
    unsigned char tracegc;
    unsigned char debugging;
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
/* interp.c */
PUBLIC void exeterm(pEnv env, Index n);
PUBLIC char *nickname(int ch);
PUBLIC char *opername(int o);
PUBLIC proc_t operproc(int o);
PUBLIC int operflags(int o);
PUBLIC int operindex(proc_t proc);
/* factor.c */
PUBLIC void readfactor(pEnv env);	/* read a JOY factor */
PUBLIC void readterm(pEnv env);
PUBLIC void writefactor(pEnv env, Index n, FILE *fp);
PUBLIC void writeterm(pEnv env, Index n, FILE *fp);
#ifdef NOBDW
PUBLIC void writedump(pEnv env, Index n, FILE *fp);
#endif
/* main.c */
PUBLIC void lookup(pEnv env);
PUBLIC void enteratom(pEnv env);
PUBLIC void abortexecution_(int num);
PUBLIC void execerror(char *str, char *op);
/* module.c */
void savemod(int *hide, int *modl, int *hcnt);
void undomod(int hide, int modl, int hcnt);
void initmod(pEnv env, char *name);
void initpriv(pEnv env);
void stoppriv(void);
void exitpriv(void);
void exitmod(void);
char *classify(pEnv env, char *name);
pEntry qualify(pEnv env, char *name);
/* scan.c */
PUBLIC void inilinebuffer(pEnv env, char *str);
PUBLIC void error(pEnv env, char *message);
PUBLIC int redirect(pEnv env, char *name, FILE *fp);
PUBLIC int include(pEnv env, char *name, int error);
PUBLIC void getsym(pEnv env);
/* utils.c */
#ifdef NOBDW
PUBLIC void inimem1(pEnv env, int status);
PUBLIC void inimem2(pEnv env);
PUBLIC void printnode(pEnv env, Index p);
PUBLIC void my_gc(pEnv env);
#endif
PUBLIC Index newnode(pEnv env, Operator o, Types u, Index r);
PUBLIC void my_memoryindex(pEnv env);
PUBLIC void my_memorymax(pEnv env);
/* quit.c */
void quit_(pEnv env);
void my_atexit(void (*proc)(pEnv));
#endif
