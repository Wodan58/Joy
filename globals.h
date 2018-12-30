/* FILE : globals.h */
#ifndef GLOBALS_H
#define GLOBALS_H
				/* configure			*/
#define SHELLESCAPE	'$'
#define INPSTACKMAX	10
#define INPLINEMAX	255
#define ALEN		20
#define HASHSIZE	9
#define SYMTABMAX	1000
#define DISPLAYMAX	10	/* nesting in HIDE & MODULE	*/
# ifndef GC_BDW
#    define MEMORYMAX	20000
# else
#    define MEMORYMAX	0
# endif
#define INIECHOFLAG	0
#define INIAUTOPUT	1
#define INITRACEGC	1
				/* installation dependent	*/
#define SETSIZE		32
#define MAXINT		2147483647
				/* symbols from getsym		*/
#define ILLEGAL_	0
#define COPIED_		1
#define USR_		2
#define ANON_FUNCT_	3
#define BOOLEAN_	4
#define CHAR_		5
#define INTEGER_	6
#define SET_		7
#define STRING_		8
#define LIST_		9
#define FLOAT_		10
#define FILE_		11
#define LBRACK		900
#define LBRACE		901
#define LPAREN		902
#define ATOM		999	/* last legal factor begin */
#define RBRACK		1001
#define RPAREN		1003
#define RBRACE		1005
#define PERIOD		1006
#define SEMICOL		1007
#define LIBRA		1100
#define EQDEF		1101
#define HIDE		1102
#define IN		1103
#define END		1104
#define MODULE		1105
#define JPRIVATE	1106
#define JPUBLIC		1107

#ifdef DEBUG
#    define D(x) x
#else
#    define D(x)
#endif

#define PRIVATE static
#define PUBLIC

				/* types			*/
typedef int Symbol;
typedef short Operator;

typedef union
      { long num;
	long set;
	char *str;
	double dbl;
	FILE *fil;
	struct Node *lis;
	struct Entry *ent;
	void (*proc)(); } Types;

typedef struct Node
  { Operator op;
    Types u;
    struct Node *next; } Node;

typedef struct Entry
  { char *name;
    int is_module;
    union
      { Node *body;
	struct Entry *module_fields;
	void  (*proc) (); } u;
    struct Entry *next; } Entry;

#ifdef ALLOC
#    define CLASS
#else
#    define CLASS extern
#endif

CLASS FILE *srcfile;
CLASS int g_argc;
CLASS char **g_argv;
CLASS int echoflag;
CLASS int autoput;
CLASS int undeferror;
CLASS int tracegc;
CLASS int startclock,gc_clock;			/* main		*/
// CLASS int ch;				/* scanner	*/
CLASS Symbol sym;
CLASS long num;
CLASS double dbl;
CLASS char id[ALEN];
CLASS int hashvalue;
CLASS Types bucket;				/* used by NEWNODE defines */
CLASS int display_enter;
CLASS int display_lookup;

CLASS Entry					/* symbol table	*/
    symtab[SYMTABMAX],
    *hashentry[HASHSIZE],
    *localentry,
    *symtabindex,
    *display[DISPLAYMAX],
    *firstlibra,				/* inioptable	*/
    *location;					/* getsym	*/

#define LOC2INT(e) (((long)e - (long)symtab) / sizeof(Entry))
#define INT2LOC(x) ((Entry*) ((x + (long)symtab)) * sizeof(Entry))

CLASS Node				/* dynamic memory	*/
/*
    memory[MEMORYMAX],
    *memoryindex,
*/
    *prog, *stk, *conts,
    *dump, *dump1, *dump2, *dump3, *dump4, *dump5;

#define MEM2INT(n) (((long)n - (long)memory) / sizeof(Node))
#define INT2MEM(x) ((Node*) ((x + (long)&memory) * sizeof(Node)))

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
PUBLIC void stack_(void);
PUBLIC void dummy_(void);
PUBLIC void exeterm(Node *n);
PUBLIC void inisymboltable(void)		/* initialise		*/;
PUBLIC char *opername(int o);
PUBLIC void lookup(void);
PUBLIC void abortexecution_(void);
PUBLIC void execerror(char *message, char *op);
PUBLIC void quit_(void);
PUBLIC void inilinebuffer(void);
PUBLIC void putline(void);
PUBLIC int endofbuffer(void);
PUBLIC void error(char *message);
PUBLIC int doinclude(char *filnam);
PUBLIC void getsym(void);
PUBLIC void inimem1(void);
PUBLIC void inimem2(void);
PUBLIC void printnode(Node *p);
PUBLIC void gc_(void);
PUBLIC Node *newnode(Operator o, Types u, Node *r);
PUBLIC void memoryindex_(void);
PUBLIC void readfactor(void)		/* read a JOY factor		*/;
PUBLIC void readterm(void);
PUBLIC void writefactor(Node *n, FILE *stm);
PUBLIC void writeterm(Node *n, FILE *stm);

#define USR_NEWNODE(u,r)	(bucket.ent = u, newnode(USR_, bucket, r))
#define ANON_FUNCT_NEWNODE(u,r)	(bucket.proc= u, newnode(ANON_FUNCT_,bucket,r))
#define BOOLEAN_NEWNODE(u,r)	(bucket.num = u, newnode(BOOLEAN_, bucket, r))
#define CHAR_NEWNODE(u,r)	(bucket.num = u, newnode(CHAR_, bucket, r))
#define INTEGER_NEWNODE(u,r)	(bucket.num = u, newnode(INTEGER_, bucket, r))
#define SET_NEWNODE(u,r)	(bucket.num = u, newnode(SET_, bucket, r))
#define STRING_NEWNODE(u,r)	(bucket.str = u, newnode(STRING_, bucket, r))
#define LIST_NEWNODE(u,r)	(bucket.lis = u, newnode(LIST_, bucket, r))
#define FLOAT_NEWNODE(u,r)	(bucket.dbl = u, newnode(FLOAT_, bucket, r))
#define FILE_NEWNODE(u,r)	(bucket.fil = u, newnode(FILE_, bucket, r))
#endif
