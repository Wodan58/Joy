/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.65
 *  date    : 08/18/23
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

#ifdef _MSC_VER
#include <io.h>
#pragma warning(disable : 4267)
#else
#include <unistd.h>
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
#endif

/*
    Note: strdup, snprintf, localtime_r, gmtime_r are sometimes not available.
*/

/*
    The following #defines are present in the source code.
    They have been accepted as an addition to the original code.
*/
#define CORRECT_INTERN_LOOKUP
#define READ_PRIVATE_AHEAD
#define SEARCH_EXEC_DIRECTORY

/*
    The following #defines are present in the source code.
    They have been accepted as original code.
*/
#define USE_SHELL_ESCAPE

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
#define SETSIZE 64
#define MAXINT 9223372036854775807LL

/* symbols from getsym		*/
#define ILLEGAL_ 0
#define COPIED_ 1
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
#define FALSE_ 12
#define TRUE_ 13
#define MAXINT_ 14
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
    OK,
    IMMEDIATE
} Flags;

/* types			*/
typedef int Symbol;
typedef int Operator;

#ifdef NOBDW
typedef unsigned Index;
#else
typedef struct Node *Index;
#endif

typedef unsigned pEntry;

typedef struct Env *pEnv;

typedef union {
    int64_t num;	/* USR, BOOLEAN, CHAR, INTEGER */
    uint64_t set;	/* SET */
    char *str;		/* STRING */
    double dbl;		/* FLOAT */
    FILE *fil;		/* FILE */
    Index lis;		/* LIST */
    pEntry ent;		/* SYMBOL */
    void (*proc)(pEnv);	/* ANON_FUNCT */
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
	void (*proc)(pEnv);
    } u;
} Entry;

typedef struct Token {
    Types yylval;
    Symbol symb;
} Token;

#include <gc.h>
#include "kvec.h"
#include "khash.h"

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
    int token_index;
    pEntry location;       /* getsym */
    Symbol symb;	   /* scanner */
    char *hide_stack[DISPLAYMAX];
    struct module {
	char *name;
	int hide;
    } module_stack[DISPLAYMAX];
    unsigned char autoput; /* options */
    unsigned char echoflag;
    unsigned char undeferror;
    unsigned char tracegc;
    unsigned char debugging;
    unsigned char token_list;
} Env;

/* GOOD REFS:
	005.133l H4732		A LISP interpreter in C
	Manna p139  recursive Ackermann SCHEMA

   OTHER DATA TYPES
	WORD = "ABCD" - up to four chars
	LIST of SETs of char [S0 S1 S2 S3]
		LISTS - binary tree [left right]
			" with info [info left right]
	STRING of 32 chars = 32 * 8 bits = 256 bits = bigset
	CHAR = 2 HEX
	32 SET = 2 * 16SET
*/

/* Public procedures: */
/* interp.c */
PUBLIC void exeterm(pEnv env, Index n);
PUBLIC char *nickname(int o);
PUBLIC char *opername(int o);
PUBLIC void (*operproc(int o))(pEnv env);
PUBLIC int operflags(int o);
PUBLIC int opertype(int o);
/* main.c */
PUBLIC void lookup(pEnv env);
PUBLIC void abortexecution_(void);
PUBLIC void execerror(char *message, char *op);
/* scan.c */
PUBLIC void my_atexit(void (*proc)(pEnv));
PUBLIC void inilinebuffer(pEnv env, char *filnam);
PUBLIC void error(pEnv env, char *message);
PUBLIC int include(pEnv env, char *filnam, int error);
PUBLIC void ungetsym(Symbol symb);
PUBLIC void getsym(pEnv env);
/* factor.c */
PUBLIC void readfactor(pEnv env, int priv); /* read a JOY factor */
PUBLIC void readterm(pEnv env, int priv);
PUBLIC void writefactor(pEnv env, Index n);
PUBLIC void writeterm(pEnv env, Index n);
PUBLIC void writedump(pEnv env, Index n);
/* module.c */
PUBLIC void initmod(pEnv env, char *name);
PUBLIC void initpriv(pEnv env, int priv);
PUBLIC void stoppriv(void);
PUBLIC void exitpriv(void);
PUBLIC void exitmod(void);
PUBLIC char *classify(pEnv env, char *name);
PUBLIC pEntry qualify(pEnv env, char *name);
/* utils.c */
PUBLIC Index newnode(pEnv env, Operator o, Types u, Index r);
PUBLIC void my_memorymax(pEnv env);
PUBLIC void my_memoryindex(pEnv env);
#ifdef NOBDW
PUBLIC void printnode(pEnv env, Index p);
PUBLIC void inimem1(pEnv env, int status);
PUBLIC void inimem2(pEnv env);
PUBLIC void my_gc(pEnv env);
#endif
#endif
