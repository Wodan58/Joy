/* FILE: globals.h */
/*
 *  module  : globals.h
 *  version : 1.24.1.7
 *  date    : 06/28/20
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#define BIT_32

#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

/*
    The following #defines are not available in the oldest ANSI standard.

#define USE_SNPRINTF
#define USE_TIME_REC
*/

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
NO_HELP_LOCAL_SYMBOLS
USE_UNKNOWN_SYMBOLS
*/
/*
    The following #defines are NOT present in the source code.
    They have NOT been accepted.

CORRECT_FLOAT_BUFFER
GET_FROM_STDIN
ORIGINAL_JOY
REMOVE_UNUSED_ERRORCOUNT
CHECK_END_SYMBOL
*/
/*
    The following #defines are present in the source code.
    They have NOT been accepted.

TRACK_USED_SYMBOLS
NOT_ALSO_FOR_FLOAT
NOT_ALSO_FOR_FILE
TRACE
STATS
ENABLE_TRACEGC
*/
/*
    The following #defines are present in the source code.
    They have been accepted.
    Note: strdup, snprintf, localtime_r, gmtime_r are not available
    when compiling with -ansi.
*/
#define FGET_FROM_FILE
#define SAMETYPE_BUILTIN
#define GETCH_AS_BUILTIN
#define ONLY_LOGICAL_NOT
#define CORRECT_FREAD_PARAM
#define CORRECT_INTERN_LOOKUP
#define REST_OF_UNIX_ESCAPES
#define CORRECT_ALEN
#define CORRECT_TYPE_COMPARE
#define CORRECT_CASE_COMPARE
#define CORRECT_INHAS_COMPARE
/*
    The following #defines are present in the source code.
    They have been accepted.
*/
#define USE_SHELL_ESCAPE
#define RUNTIME_CHECKS

/* configure			*/
#define SHELLESCAPE '$'
#define INPSTACKMAX 10
#define INPLINEMAX 255
#ifdef CORRECT_ALEN
#define ALEN 22
#else
#define ALEN 20
#endif
#define HASHSIZE 9
#define SYMTABMAX 1000
#define DISPLAYMAX 10 /* nesting in HIDE & MODULE	*/
#define MEMORYMAX 20006
#define INIECHOFLAG 0
#define INIAUTOPUT 1
#define INITRACEGC 1

/* installation dependent	*/
#ifdef BIT_32
#define SETSIZE 32
#define MAXINT 2147483647
typedef int long_t;
#else
#define SETSIZE 64
#define MAXINT 9223372036854775807LL
typedef long long long_t;
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

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

#if 0
#define PRIVATE static
#else
#define PRIVATE
#endif
#define PUBLIC

/* types			*/
typedef int Symbol;

typedef short Operator;

typedef union {
    long_t num;
    long_t set;
    char* str;
    double dbl;
    FILE* fil;
    struct Node* lis;
    struct Entry* ent;
    void (*proc)();
} Types;

typedef struct Node {
    Types u;
    Operator op;
    struct Node* next;
} Node;

typedef struct Entry {
    char* name;
    unsigned char is_local;
    unsigned char is_module;
#ifdef TRACK_USED_SYMBOLS
    unsigned char is_used;
#endif
    union {
        Node* body;
        struct Entry* module_fields;
        void (*proc)();
    } u;
    struct Entry* next;
} Entry;

#ifdef ALLOC
#define CLASS
#else
#define CLASS extern
#endif

CLASS FILE* srcfile;
CLASS int g_argc;
CLASS char** g_argv;
CLASS int echoflag;
CLASS int autoput;
CLASS int undeferror;
CLASS int tracegc;
CLASS int startclock, gc_clock; /* main */
#if 0
CLASS int ch;					/* scanner */
#endif
CLASS Symbol symb;
CLASS long_t numb;
CLASS char* strg; /* string */
CLASS double dblf;
CLASS char ident[ALEN];
CLASS int hashvalue;
CLASS Types bucket; /* used by NEWNODE defines */
CLASS int display_enter;
CLASS int display_lookup;

CLASS Entry /* symbol table */
    symtab[SYMTABMAX],
    *hashentry[HASHSIZE],
#if 0
    *localentry,
#endif
    *symtabindex, *display[DISPLAYMAX], *firstlibra, /* inioptable */
    *location; /* getsym */

#define LOC2INT(e) (((size_t)e - (size_t)symtab) / sizeof(Entry))
#define INT2LOC(x) ((Entry*)((x + (size_t)symtab)) * sizeof(Entry))

CLASS Node /* dynamic memory	*/
    /*
        memory[MEMORYMAX],
        *memoryindex,
    */
    *prog,
    *stck, *conts, *dump, *dump1, *dump2, *dump3, *dump4, *dump5;

#define MEM2INT(n) (((size_t)n - (size_t)memory) / sizeof(Node))
#define INT2MEM(x) ((Node*)((x + (size_t)memory) * sizeof(Node)))

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
#if 0
PUBLIC void stack_(void);
PUBLIC void dummy_(void);
#endif
PUBLIC void exeterm(Node* n);
PUBLIC void inisymboltable(void); /* initialise */
PUBLIC char* opername(int o);
PUBLIC void lookup(int priv);
PUBLIC void abortexecution_(void);
PUBLIC void execerror(char* message, char* op);
PUBLIC void quit_(void);
PUBLIC void inilinebuffer(char* filnam);
PUBLIC int getlinenum(void);
PUBLIC void resetlinebuffer(int linenum);
#if 0
PUBLIC void putline(void);
PUBLIC int endofbuffer(void);
#endif
PUBLIC void error(char* message);
PUBLIC int doinclude(char* filnam);
PUBLIC void getsym(void);
PUBLIC void inimem1(void);
PUBLIC void inimem2(void);
PUBLIC void printnode(Node* p);
PUBLIC void gc_(void);
PUBLIC Node* newnode(Operator o, Types u, Node* r);
PUBLIC void memoryindex_(void);
PUBLIC void readfactor(int priv); /* read a JOY factor */
PUBLIC void readterm(int priv);
PUBLIC void writefactor(Node* n, FILE* stm);
PUBLIC void writeterm(Node* n, FILE* stm);

#ifdef FGET_FROM_FILE
PUBLIC void redirect(FILE*);
#endif

PUBLIC void HashValue(char* str);

#define NEWNODE(o, u, r) (bucket.lis = newnode(o, u, r), bucket.lis)
#define USR_NEWNODE(u, r) (bucket.ent = u, NEWNODE(USR_, bucket, r))
#define ANON_FUNCT_NEWNODE(u, r)                                               \
    (bucket.proc = u, NEWNODE(ANON_FUNCT_, bucket, r))
#define BOOLEAN_NEWNODE(u, r) (bucket.num = u, NEWNODE(BOOLEAN_, bucket, r))
#define CHAR_NEWNODE(u, r) (bucket.num = u, NEWNODE(CHAR_, bucket, r))
#define INTEGER_NEWNODE(u, r) (bucket.num = u, NEWNODE(INTEGER_, bucket, r))
#define SET_NEWNODE(u, r) (bucket.num = u, NEWNODE(SET_, bucket, r))
#define STRING_NEWNODE(u, r) (bucket.str = u, NEWNODE(STRING_, bucket, r))
#define LIST_NEWNODE(u, r) (bucket.lis = u, NEWNODE(LIST_, bucket, r))
#define FLOAT_NEWNODE(u, r) (bucket.dbl = u, NEWNODE(FLOAT_, bucket, r))
#define FILE_NEWNODE(u, r) (bucket.fil = u, NEWNODE(FILE_, bucket, r))
#endif
