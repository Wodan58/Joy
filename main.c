/* FILE : main.c */

/*
I comment on the functions in main.c which are relevant to the
symbol table management. Comments are in the same order as they
occur in main.c.

function enterglobal:

Used for most definitions, except for those inside a HIDE,
PRIVATE, PUBLIC, MODULE. Probably this should be implemented
first.
For a definition "foo == ...", enters identifier id
(string "foo") into global table. For a first use of "bar" inside
"foo == ..bar.." enters "bar" into global table. In my implementation this
part of the table uses a hashing method, the actual hash value
has already been computed by the scanner.

function lookup:

Since HIDE's and modules can be nested, there is the notion
of level (up to 10, which is ample), level 0 means global).
The array display contains the starting points of the local
tables. So, to lookup identifier id (string "foo"), start
at the current highest level (display_lookup), do a linear
search at that local table. If not found, go down one level
and repeat. If found on a local level, return (global variable
location now knows where "foo" is.
If "foo" was not found locally, search the global table which
is hashed: another linear search through the list for this
particular hash value. If not found enter into global table.
ALSO NOTE: my implementation fails to enter "foo" flagged
as is_module = 0 in the normal case. It so happens that
memory is automatically initialised to 0, so it worked. But
this was bad programming. Sorry.


function enteratom:

If we are inside a HIDE, PRIVATE or a module and see "foo == ..."
enter "foo" into table (with a NULL body), and add this
entry to the current local table at the current display level.

function definition:

Remember from that a definition is either simple (of the
form "foo == ..") or compound (HIDE, MODULE etc).
In the body of the function the first 6 lines are just for
parsing the compound ones, in each case calling function
compound_def.
The next few lines deal with empty definitions, which should
be allowed so that ";" can be used as a terminator and not just
as a separator between definitions (Pascal uses a similar trick
for statement terminator/separator).
The remainder deals with the case of atom "foo". The warning for
overwriting inbuilt should be clear, also the parsing for "==".
Then a term "..." is read, which readterm always leaves on the stack.
That is term, on top of the stack, is then attached as the body
to the part of the symbol table at here.

function defsequence:

This just parses definition sequences separated by ";".

function compound_def:

This processes in accordance with the grammar
compound-definition :==
["MODULE" atomic symbol]
["PRIVATE" definition-sequence ]
                               ^ (* ERROR: MISSING "compound_def"
                                    SEE DISGUST BELOW *)
["PUBLIC" definition-sequence]
("END" | ".")
Each of the 3 parts is optional. Instead of "PRIVATE.. PUBLIC.."
the combination "HIDE..IN.." can be used, and instead of
"PUBLIC" the syntax "LIBRA.." or "DEFINE.." can be used.
So, depending on what the symbol from the scanner is:

If the symbol is "MODULE", the next symbol must be an atom,
say "mymodule", enter it as an atom into the global or
local part of the symbol table, increment two display
indices for entering and lookup, then parse a compound definition,
which will be at one level higher than the preceding.
and then: set the is_module flag to 1 and the fields
to the current value. Finally decrement these two indices.
If the symbol is "PRIVATE" or "HIDE", the next item to be
parsed has to be a definition sequence. But the details
of what to do with the display indices depend on whether
we are already inside something that is hidden. If we are,
then only the display_enter is incremented before the
definition sequence and decremented afterwards. If we are not,
then both indices are incremented and decremented at the same
place. In either case after all that the function calls
itself. [NOTE: TO MY DISGUST, THE GRAMMAR IN THE MANUAL DOES
NOT SAY THIS - VERY SORRY.]
If the symbol is "PUBLIC" or "LIBRA" or "IN", parse and
process a definition-sequence.
If the symbol is anything else, warn about empty
compound definition.

I suppose part of the difficulty of understanding this function
is due to the fact that I tried to squeeze the earlier "HIDE..IN.."
into the same mould as the quite separate "PRIVATE.." and "PUBLIC..",
and trying to use the same display mechanism for the levels of
hiding by "HIDE" or "PRIVATE". Maybe there is a more elegant way
of doing this, possibly by separating the cases and duplicating
some of the code.

Manfred von Thun, 2006
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>
#define ALLOC
#include "globals.h"

PRIVATE void enterglobal()
{
    location = symtabindex++;
D(  printf("getsym, new: '%s'\n",id); )
    location->name = (char *) malloc(strlen(id) + 1);
    strcpy(location->name,id);
    location->u.body = NULL; /* may be assigned in definition */
    location->next = hashentry[hashvalue];
D(  printf("entered %s at %ld\n",id,LOC2INT(location)); )
    hashentry[hashvalue] = location;
}

PUBLIC void lookup(void)
{
int i;
D(  printf("%s  hashes to %d\n",id,hashvalue); )

    for (i = display_lookup; i > 0; --i)
      { location = display[i];
	while (location != NULL && strcmp(id,location->name) != 0)
	    location = location->next;
	if (location != NULL) /* found in local table */
	    return; }

    location = hashentry[hashvalue];
    while (location != symtab &&
	   strcmp(id,location->name) != 0)
	location = location->next;

    if (location == symtab) /* not found, enter in global */
	enterglobal();
}

PRIVATE void enteratom()
{
    if (display_enter > 0)
      { location = symtabindex++;
D(	printf("hidden definition '%s' at %ld\n",id,LOC2INT(location)); )
	location->name = (char *) malloc(strlen(id) + 1);
	strcpy(location->name, id);
	location->u.body = NULL; /* may be assigned later */
	location->next = display[display_enter];
	display[display_enter] = location; }
    else lookup();
}

PRIVATE void defsequence();		/* forward */
PRIVATE void compound_def();		/* forward */

PRIVATE void definition()
{
    Entry *here = NULL;
    if (sym == LIBRA || sym == JPRIVATE || sym == HIDE || sym == MODULE)
      { compound_def();
	if (sym == END || sym == PERIOD) getsym();
	    else error(" END or period '.' expected in compound definition");
	return; }

    if (sym != ATOM)
/*   NOW ALLOW EMPTY DEFINITION:
      { error("atom expected at start of definition");
	abortexecution_(); }
*/
	return;

    /* sym == ATOM : */
    enteratom();
    if (location < firstlibra)
      { printf("warning: overwriting inbuilt '%s'\n",location->name);
	enterglobal(); }
    here = location; getsym();
    if (sym == EQDEF) getsym();
	else error(" == expected in definition");
    readterm();
D(  printf("assigned this body: "); )
D(  writeterm(stk->u.lis, stdout); )
D(  printf("\n"); )
    if (here != NULL)
      { here->u.body = stk->u.lis; here->is_module = 0; }
    stk = stk->next;
}

PRIVATE void defsequence()
{
    definition();
    while (sym == SEMICOL)
      { getsym(); definition(); }
}

PRIVATE void compound_def()
{
    switch (sym)
      { case MODULE :
	  { Entry *here = NULL;
	    getsym();
	    if (sym != ATOM)
	      { error("atom expected as name of module");
		abortexecution_(); }
	    enteratom(); here = location; getsym();
	    ++display_enter; ++display_lookup;
	    display[display_enter] = NULL;
	    compound_def();
	    here->is_module = 1;
	    here->u.module_fields = display[display_enter];
	    --display_enter; --display_lookup;
	    break; }
	case JPRIVATE : case HIDE :
	  { getsym();
	    if (display_lookup > display_enter)
		 /* already inside module or hide */
	      { Entry *oldplace = display[display_lookup];
/*
		printf("lookup = %d\n",LOC2INT(display[display_lookup]));
		printf("enter = %d\n",LOC2INT(display[display_enter]));
*/
		++display_enter;
		defsequence();
		--display_enter;
/*
		printf("lookup = %d\n",LOC2INT(display[display_lookup]));
		printf("enter = %d\n",LOC2INT(display[display_enter]));
*/
		compound_def();
		display[display_lookup] = oldplace;
	      }
	    else
	      { ++display_enter; ++display_lookup;
		display[display_enter] = NULL;
		defsequence();
		--display_enter;
		compound_def();
		--display_lookup; }
	    break; }
	case JPUBLIC : case LIBRA : case IN :
	  { getsym();
	    defsequence();
	    break; }
	default :
	    printf("warning: empty compound definition\n"); }
}

jmp_buf begin;
jmp_buf fail;

PUBLIC void abortexecution_(void)
{
    conts = dump = dump1 = dump2 = dump3 = dump4 = dump5 = NULL;
    longjmp(begin,0);
}

PUBLIC void fail_(void)
{
    longjmp(fail,1);
}

PUBLIC void execerror(char *message, char *op)
{
    printf("run time error: %s needed for %s\n",message,op);
    abortexecution_();
}

static int quit_quiet = 1;
   /* was = 0;  but anything with "clock" needs revision */

PUBLIC void quit_(void)
{
    long totaltime;
    if (quit_quiet) exit(0);
    totaltime = clock() - startclock;
#ifdef GC_BDW
    printf("Time:  %ld CPU\n", totaltime);
#else
    printf("time:  %ld CPU,  %d gc (= %ld%%)\n",
	totaltime, gc_clock,
	totaltime ? (1004*gc_clock)/(10*totaltime) : 0);
#endif
    exit(0);
}

static int mustinclude = 1;

#define CHECK(D,NAME)						\
    if (D)							\
      { printf("->  %s is not empty:\n",NAME);			\
	writeterm(D, stdout); printf("\n"); }

int main(int argc, char **argv)
{
#if 0
    int ch;
#endif
    g_argc = argc;
    g_argv = argv;
    if (argc > 1) {
/*
	FILE *f;
*/
	g_argc--;
	g_argv++;
	srcfile = fopen(argv[1], "r");
	if (!srcfile) {
	    printf("failed to open the file '%s'.\n", argv[1]);
	    exit(1);
	}
    } else {
	srcfile = stdin;
#ifdef GC_BDW
	printf("JOY  -  compiled at %s on %s (BDW)\n",__TIME__,__DATE__);
#else
	printf("JOY  -  compiled at %s on %s (NOBDW)\n",__TIME__,__DATE__);
#endif
	printf("Copyright 2001 by Manfred von Thun\n");
    }
    startclock = clock();
    gc_clock = 0;
    echoflag = INIECHOFLAG;
    tracegc = INITRACEGC;
    autoput = INIAUTOPUT;
#if 0
    ch = ' ';
#endif
    inilinebuffer();
    inisymboltable();
    display[0] = NULL;
    inimem1(); inimem2();
    setjmp(begin);
    setjmp(fail);
D(  printf("starting main loop\n"); )
    while (1)
     { if (mustinclude)
	  { mustinclude = 0;
	    if (fopen("usrlib.joy","r"))
	        doinclude("usrlib.joy"); }
	getsym();

	if (sym == LIBRA || sym == HIDE || sym == MODULE )
	  { inimem1();
	    compound_def();
	    inimem2(); }

	else

	  { readterm();
D(	    printf("program is: "); writeterm(stk->u.lis, stdout); printf("\n"); )
	    if (stk) {
	    prog = stk->u.lis;
	    stk = stk->next;
	    conts = NULL;
	    exeterm(prog); }
	    if (conts || dump || dump1 || dump2 || dump3 || dump4 || dump5)
	      { printf("the dumps are not empty\n");
		CHECK(conts,"conts");
		CHECK(dump,"dump"); CHECK(dump1,"dump1");
		CHECK(dump2,"dump2"); CHECK(dump3,"dump3");
		CHECK(dump4,"dump4"); CHECK(dump5,"dump5"); }
	    if (autoput == 2 && stk != NULL)
	      { writeterm(stk, stdout); printf("\n"); }
	    else if (autoput == 1 && stk != NULL)
	      { writefactor(stk, stdout); printf("\n"); stk = stk->next; } }

	 if (sym != END && sym != PERIOD)
	     error(" END or period '.' expected"); }
}
