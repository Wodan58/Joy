/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.37
 *  date    : 06/28/21
 */
#ifndef GLOBALS_H
#define GLOBALS_H

/* #define BIT_32	*/

#ifdef NULL
#undef NULL
#define NULL 0
#endif

#ifdef NOBDW
#define nodetype(n)	env->memory[(int)n].op
#define nodevalue(n)	env->memory[(int)n].u
#define nextnode1(n)	env->memory[(int)n].next
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

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

/*
    The following #defines are not available in the oldest ANSI standard.
*/
#define USE_SNPRINTF

/*
    The following #defines are NOT present in the source code.
    They have been accepted.

USE_ONLY_STDIN
NO_COMPILER_WARNINGS
NO_BANG_AS_PERIOD
PROTECT_BUFFER_OVERFLOW
DONT_READ_PAST_EOF
CORRECT_GARBAGE_COLLECTOR
READ_NUMBER_AND_STOP
READ_HEXADECIMAL_NUMBER
CORRECT_STRING_WRITE
BDW_ALSO_IN_MAIN
SIGNAL_WRITE_PIPE
NO_JUMP_FAIL
NO_DUPLICATE_CH
USE_NEW_FUNCTION_SYNTAX
CORRECT_OCTAL_NUMBER
NO_EXECUTE_ERROR
DONT_ADD_MODULE_NAMES
CHECK_SYMTABMAX
CHECK_DISPLAYMAX
HASHVALUE_FUNCTION
NO_WASTE_FP
RESPECT_INPLINEMAX
CLOSE_INCLUDE_FILES
CORRECT_GETENV
RESUME_EARLIER_LINENUM
CHECK_QUOTE_IN_SPLIT
CORRECT_HELPDETAIL
CHECK_QUOTES_IN_PRIMREC
CORRECT_HEADERS
HELP_CONDNESTREC
CORRECT_BINREC_HELP
CORRECT_FSEEK_MANUAL
CORRECT_HELP_FREMOVE
CORRECT_CLOCK_SECONDS
CORRECT_TIME_LIST
CORRECT_APP1_DOC
CORRECT_GENREC_HELP
CORRECT_TREEREC_HELP
CORRECT_TREEGENREC_HELP
CORRECT_MODF_CHECK
CORRECT_FORMATF_MESSAGE
TAILREC_CHECK_QUOTES
TREEREC_CHECK_QUOTES
TREEGENREC_CHECK_QUOTES
CORRECT_FIVE_PARAMS
CORRECT_INTERN_STRCPY
CORRECT_NEG_INTEGER
CORRECT_STRFTIME_BUF
CORRECT_FGETS
CORRECT_TAKE_STRING
CORRECT_STRING_CONCAT
CORRECT_NOT_FOR_LIST
CORRECT_SIZE_CHECK
SECURE_PUTCHARS
CORRECT_PRIMREC
CORRECT_TREEGENREC_AUX
CORRECT_TREEREC_AUX
CHECK_EMPTY_STACK
CORRECT_NULL_CASES
MAKE_CONTS_OBSOLETE
NO_HELP_LOCAL_SYMBOLS
USE_UNKNOWN_SYMBOLS
CORRECT_TYPE_COMPARE
CORRECT_CASE_COMPARE
CORRECT_INHAS_COMPARE
RUNTIME_CHECKS
ONLY_LOGICAL_NOT
CORRECT_FREAD_PARAM
CORRECT_INTERN_LOOKUP
REST_OF_UNIX_ESCAPES
CORRECT_ALEN
SAMETYPE_BUILTIN
GETCH_AS_BUILTIN
*/

/*
    The following #defines are NOT present in the source code.
    They have NOT been accepted.

CORRECT_FLOAT_BUFFER
GET_FROM_STDIN
ORIGINAL_JOY
REMOVE_UNUSED_ERRORCOUNT
CHECK_END_SYMBOL
NOT_ALSO_FOR_FLOAT
NOT_ALSO_FOR_FILE
DEBUG
*/

/*
    The following #defines are present in the source code.
    They have NOT been accepted.

TRACK_USED_SYMBOLS
TRACING
STATS
ENABLE_TRACEGC
*/

/*
    The following #defines are present in the source code.
    They have been accepted as an addition to the original code.
    Note: strdup, snprintf, localtime_r, gmtime_r are not available
    when compiling with -ansi.
*/
#define FGET_FROM_FILE

/*
    The following #defines are present in the source code.
    They have been accepted as original code.
*/
#define USE_SHELL_ESCAPE

/* configure			*/
#define SHELLESCAPE '$'
#define INPSTACKMAX 10
#define INPLINEMAX 255
#define ALEN 22
#define SYMTABMAX 0
#define DISPLAYMAX 10 /* nesting in HIDE & MODULE	*/
#define MEMORYMAX 20000
#define INIECHOFLAG 0
#define INIAUTOPUT 1
#define INITRACEGC 1

/* installation dependent	*/
#ifdef BIT_32
#define SETSIZE 32
#define MAXINT 2147483647
typedef int long_t;
typedef float my_float_t;
#else
#define SETSIZE 64
#define MAXINT 9223372036854775807
typedef long long_t;
typedef double my_float_t;
#endif

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

/* types			*/
typedef int Symbol;

#ifdef BIT_32
typedef short Operator;
#else
typedef int Operator;
#endif

#ifdef NOBDW
#ifdef BIT_32
typedef unsigned short Index;
#else
typedef unsigned Index;
#endif
#else
typedef struct Node *Index;
#endif

#ifdef NOBDW
typedef Index pEntry;
#else
typedef unsigned pEntry;
#endif

typedef struct Env *pEnv;

typedef union {
    long_t num;
    long_t set;
    char *str;
    my_float_t dbl;
    FILE *fil;
    Index lis;
    pEntry ent;
    void (*proc)(pEnv);
} Types;

typedef struct Node {
    Types u;
    Operator op;
    Index next;
} Node;

typedef struct Entry {
    char *name;
    unsigned char is_local;
    unsigned char is_module;
#ifdef TRACK_USED_SYMBOLS
    unsigned char is_used;
#endif
    pEntry next;
    union {
        Index body;
        pEntry module_fields;
        void (*proc)(pEnv);
    } u;
} Entry;

#include "khash.h"
#include "kvec.h"

KHASH_MAP_INIT_STR(Symtab, pEntry)

typedef struct Env {
#ifdef NOBDW
    Node *memory;
#endif
    vector(Entry) *symtab;
    khash_t(Symtab) *hash;
#ifdef NOBDW
    Index prog, stck, conts, dump, dump1, dump2, dump3, dump4, dump5;
#else
    Node *prog, *stck;
#endif
    Types yylval, bucket;
} Env;

#ifdef ALLOC
#define CLASS
#else
#define CLASS extern
#endif

CLASS FILE *srcfile;
CLASS int g_argc;
CLASS char **g_argv;
CLASS int echoflag;
CLASS int autoput;
CLASS int undeferror;
CLASS int startclock; /* main */
#ifdef NOBDW
CLASS int tracegc;
CLASS int gc_clock;
#endif
CLASS Symbol symb; /* scanner */
CLASS char ident[ALEN];
CLASS int display_enter;
CLASS int display_lookup;

CLASS pEntry /* symbol table */
    symtabindex,
    display[DISPLAYMAX], firstlibra, /* inioptable */
    location; /* getsym */

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
PUBLIC void exeterm(pEnv env, Index n);
PUBLIC void inisymboltable(pEnv env); /* initialise */
PUBLIC char *opername(int o);
PUBLIC void (*operproc(int o))(pEnv env);
PUBLIC void lookup(pEnv env, int priv);
PUBLIC void abortexecution_(pEnv env);
PUBLIC void execerror(pEnv env, char *message, char *op);
PUBLIC void quit_(pEnv env);
PUBLIC void inilinebuffer(char *filnam);
PUBLIC int getlinenum(void);
PUBLIC void resetlinebuffer(int linenum);
PUBLIC void error(char *message);
PUBLIC void doinclude(pEnv env, char *filnam);
PUBLIC void getsym(pEnv env);
#ifdef NOBDW
PUBLIC void inimem1(pEnv env);
PUBLIC void inimem2(pEnv env);
PUBLIC void my_gc(pEnv env);
#endif
PUBLIC void printnode(pEnv env, Index p);
PUBLIC Index newnode(pEnv env, Operator o, Types u, Index r);
PUBLIC void readfactor(pEnv env, int priv); /* read a JOY factor */
PUBLIC void readterm(pEnv env, int priv);
PUBLIC void my_writefactor(pEnv env, Node *n, FILE *stm);
#ifdef NOBDW
PUBLIC void my_memorymax(pEnv env);
PUBLIC void my_memoryindex(pEnv env);
#endif
PUBLIC void writefactor(pEnv env, Index n, FILE *stm);
PUBLIC void writeterm(pEnv env, Index n, FILE *stm);

#ifdef FGET_FROM_FILE
PUBLIC void redirect(pEnv env, FILE *);
#endif

#define NEWNODE(o, u, r)                                                       \
    (env->bucket.lis = newnode(env, o, u, r), env->bucket.lis)
#define USR_NEWNODE(u, r) (env->bucket.ent = u, NEWNODE(USR_, env->bucket, r))
#define ANON_FUNCT_NEWNODE(u, r)                                               \
    (env->bucket.proc = u, NEWNODE(ANON_FUNCT_, env->bucket, r))
#define BOOLEAN_NEWNODE(u, r)                                                  \
    (env->bucket.num = u, NEWNODE(BOOLEAN_, env->bucket, r))
#define CHAR_NEWNODE(u, r)                                                     \
    (env->bucket.num = u, NEWNODE(CHAR_, env->bucket, r))
#define INTEGER_NEWNODE(u, r)                                                  \
    (env->bucket.num = u, NEWNODE(INTEGER_, env->bucket, r))
#define SET_NEWNODE(u, r) (env->bucket.num = u, NEWNODE(SET_, env->bucket, r))
#define STRING_NEWNODE(u, r)                                                   \
    (env->bucket.str = u, NEWNODE(STRING_, env->bucket, r))
#define LIST_NEWNODE(u, r)                                                     \
    (env->bucket.lis = u, NEWNODE(LIST_, env->bucket, r))
#define FLOAT_NEWNODE(u, r)                                                    \
    (env->bucket.dbl = u, NEWNODE(FLOAT_, env->bucket, r))
#define FILE_NEWNODE(u, r)                                                     \
    (env->bucket.fil = u, NEWNODE(FILE_, env->bucket, r))
#endif
