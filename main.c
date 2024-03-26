/* FILE: main.c */
/*
 *  module  : main.c
 *  version : 1.94
 *  date    : 03/23/24
 */

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
of level (up to 10, which is ample, level 0 means global).
The array display contains the starting points of the local
tables. So, to lookup identifier id (string "foo"), start
at the current highest level (display_lookup), do a linear
search at that local table. If not found, go down one level
and repeat. If found on a local level, return (global variable
location now knows where "foo" is).
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
#include "globals.h"

/*
 * DUMP_CHECK - check that the dumps are not empty.
 */
#define DUMP_CHECK(D, NAME)						\
    if (D) {								\
	printf("->  %s is not empty:\n", NAME);				\
	writedump(&env, D, stdout);					\
	putchar('\n');							\
    }

static jmp_buf begin;		/* restart with empty program */

char *bottom_of_stack;		/* needed in gc.c */

/*
 * abort execution and restart reading from srcfile; the stack is not cleared.
 */
void abortexecution_(int num)
{
    longjmp(begin, num);
}

/*
 * options - print help on startup options and exit: options are those that
 *	     cannot be set from within the language itself.
 */
static void options()
{
    printf("JOY  -  compiled at %s on %s", __TIME__, __DATE__);
#ifdef VERS
    printf(" (%s)", VERS);
#endif
    printf("\nCopyright 2001 by Manfred von Thun\n");
    printf("Usage: joy (options | filenames | parameters)*\n");
    printf("options, filenames, parameters can be given in any order\n");
    printf("options start with '-' and parameters start with a digit\n");
    printf("filenames can be preceded by a pathname: e.g. './'\n");
#ifdef COMP
    printf("Compile flags: %s\n", COMP);
#endif
#ifdef LINK
    printf("Linker flags: %s\n", LINK);
#endif
    printf("Options:\n");
    printf("  -a : set the autoput flag (0-2)\n");
    printf("  -d : print a trace of stack development\n");
    printf("  -e : set the echoflag (0-3)\n");
    printf("  -g : set the __tracegc flag (0-6)\n");
    printf("  -h : print this help text and exit\n");
    printf("  -i : ignore impure imperative functions\n");
    printf("  -l : do not read usrlib.joy at startup\n");
    printf("  -p : print debug list of tokens\n");
    printf("  -s : dump symbol table after execution\n");
    printf("  -t : print a trace of program execution\n");
    printf("  -u : set the undeferror flag (0,1)\n");
    printf("  -w : suppress warnings overwrite, arity\n");
    printf("  -x : print statistics after program end\n");
}

/*
 * opt_unknown - report unknown option and point out -h option.
 */
static void opt_unknown(char *exe, int ch)
{
    printf("Unknown option argument: \"-%c\"\n", ch);
    printf("More info with: \"%s -h\"\n", exe);
}

static int my_main(int argc, char **argv)
{
    static unsigned char psdump = 0, pstats = 0;
    Env env;		/* global variables */
    FILE *srcfile;
    char *filenam;
    char *ptr, *exe;	/* exe: name of joy binary */
    int i, j, ch, flag;
    unsigned char mustinclude = 1, helping = 0, unknown = 0;

    memset(&env, 0, sizeof(env));
    /*
     * Start the clock.
     */
    env.startclock = clock();
    /*
     * determine srcfile and filenam; they are stored in inilinebuffer.
     */
    srcfile = stdin;
    filenam = "stdin";
    /*
     * establish pathname, to be used when loading libraries, and basename.
     */
    if ((ptr = strrchr(argv[0], '/')) != 0 ||
	(ptr = strrchr(argv[0], '\\')) != 0) {
	*ptr++ = 0;
	env.pathname = argv[0];
	argv[0] = ptr;
    } else
	env.pathname = ".";
    exe = argv[0];
    /*
     * These flags are initialized here, allowing them to be overruled by the
     * command line. When set on the command line, they can not be overruled
     * in the source code.
     */
    env.autoput = INIAUTOPUT;
    env.echoflag = INIECHOFLAG;
    env.undeferror = INIUNDEFERROR;
    env.tracegc = INITRACEGC;
    env.overwrite = INIWARNING;
    /*
     * First look for options. They start with -.
     */
    for (i = 1; i < argc; i++) {
	if (argv[i][0] == '-') {
	    for (j = 1; argv[i][j]; j++) {
		switch (argv[i][j]) {
		case 'a' : ptr = &argv[i][j + 1];
			   env.autoput = atoi(ptr);	/* numeric payload */
			   env.autoput_set = 1;
			   ch = *ptr;			/* first digit */
			   while (isdigit(ch)) {
			       j++;			/* point last digit */
			       ptr++;
			       ch = *ptr;
			   }
			   break;
		case 'd' : env.debugging = 1; break;
		case 'e' : ptr = &argv[i][j + 1];
			   env.echoflag = atoi(ptr);	/* numeric payload */
			   env.echoflag_set = 1;
			   ch = *ptr;			/* first digit */
			   while (isdigit(ch)) {
			       j++;			/* point last digit */
			       ptr++;
			       ch = *ptr;
			   }
			   break;
		case 'g' : ptr = &argv[i][j + 1];
			   env.tracegc = atoi(ptr);	/* numeric payload */
			   ch = *ptr;			/* first digit */
			   while (isdigit(ch)) {
			       j++;			/* point last digit */
			       ptr++;
			       ch = *ptr;
			   }
			   break;
		case 'h' : helping = 1; break;
		case 'i' : env.ignore = 1; break;
		case 'l' : mustinclude = 0; break;
		case 'p' : env.printing = 1; break;
		case 's' : psdump = 1; break;
		case 't' : env.debugging = 2; break;
		case 'u' : ptr = &argv[i][j + 1];
			   env.undeferror = atoi(ptr);	/* numeric payload */
			   env.undeferror_set = 1;
			   ch = *ptr;			/* first digit */
			   while (isdigit(ch)) {
			       j++;			/* point last digit */
			       ptr++;
			       ch = *ptr;
			   }
			   break;
		case 'w' : env.overwrite = 0; break;
		case 'x' : pstats = 1; break;
		default  : unknown = argv[i][j]; break;
		} /* end switch */
	    } /* end for */
	    /*
	     * Overwrite the option(s) with subsequent parameters. Index i is
	     * decreased, because the next parameter is copied to the current
	     * index and i is increased in the for-loop.
	     */
	    for (--argc, j = i--; j < argc; j++)
		argv[j] = argv[j + 1];
	} /* end if */
    } /* end for */
    /*
     * Look for a possible filename parameter. Filenames cannot start with -
     * and cannot start with a digit, unless preceded by a path: e.g. './'.
     */
    for (i = 1; i < argc; i++) {
	ch = argv[i][0];
	if (!isdigit(ch)) {
	    if ((srcfile = fopen(filenam = argv[i], "r")) == 0) {
		fprintf(stderr, "failed to open the file '%s'.\n", filenam);
		return 0;
	    }
	    /*
	     *  Overwrite argv[0] with the filename and shift subsequent
	     *  parameters.
	     */
	    if ((ptr = strrchr(argv[0] = filenam, '/')) != 0) {
		*ptr++ = 0;
		argv[0] = filenam = ptr; /* basename */
	    }
	    for (--argc; i < argc; i++)
		argv[i] = argv[i + 1];
	    break;
	} /* end if */
    } /* end for */
    inilinebuffer(srcfile, filenam);
    /*
     * determine argc and argv.
     */
    env.g_argc = argc;
    env.g_argv = argv;
    /*
     * initialize vectors.
     */
    vec_init(env.string);
    vec_init(env.pushback);
    vec_init(env.tokens);
    vec_init(env.symtab);
    /*
     * initialize symbol table.
     */
    inisymboltable(&env);
    /*
     * handle options, might print symbol table.
     */
    if (helping || unknown) {
	helping ? options() : opt_unknown(exe, unknown);
	goto einde;
    }
    /*
     * initialize standard output.
     */
    setbuf(stdout, 0);		/* necessary when writing to a pipe */
    /*
     * read initial library.
     */
    if (mustinclude)
	include(&env, "usrlib.joy");
    /*
     * setup return address of error, abort, or quit.
     */
    if (setjmp(begin) == ABORT_QUIT)
	goto einde;		/* return here after error or abort */
#ifdef NOBDW
    inimem1(&env, 0);		/* does not clear the stack */
    inimem2(&env);
#endif
    /*
     * (re)initialize code.
     */
    env.prog = 0;		/* clear program, just to be sure */
    ch = getch(&env);
    while (1) {
	ch = getsym(&env, ch);
	if (env.sym == LIBRA || env.sym == HIDE || env.sym == MODULE_ ||
	    env.sym == CONST_) {
#ifdef NOBDW
	    inimem1(&env, 1);	/* also clears the stack */
#endif
	    if ((flag = env.sym == MODULE_) != 0)
		hide_inner_modules(&env, 1);
	    ch = compound_def(&env, ch);
	    if (flag)
		hide_inner_modules(&env, 0);
#ifdef NOBDW
	    inimem2(&env);	/* enlarge definition space */
#endif
	} else {
	    ch = readterm(&env, ch);
#ifdef NOBDW
	    if (env.stck && vec_at(env.memory, env.stck).op == LIST_) {
		env.prog = vec_at(env.memory, env.stck).u.lis;
		env.stck = vec_at(env.memory, env.stck).next;
		env.conts = 0;
#else
	    if (env.stck && nodetype(env.stck) == LIST_) {
		env.prog = nodevalue(env.stck).lis;
		env.stck = nextnode1(env.stck);
#endif
		exeterm(&env, env.prog);
	    }
#ifdef NOBDW
	    if (env.conts || env.dump || env.dump1 || env.dump2 || env.dump3
		|| env.dump4 || env.dump5) {
		printf("the dumps are not empty\n");
		DUMP_CHECK(env.conts, "conts");
		DUMP_CHECK(env.dump, "dump");
		DUMP_CHECK(env.dump1, "dump1");
		DUMP_CHECK(env.dump2, "dump2");
		DUMP_CHECK(env.dump3, "dump3");
		DUMP_CHECK(env.dump4, "dump4");
		DUMP_CHECK(env.dump5, "dump5");
	    }
#endif
	    if (env.stck) {
		if (env.autoput == 2)
		    writeterm(&env, env.stck, stdout);
		else if (env.autoput == 1) {
		    writefactor(&env, env.stck, stdout);
#ifdef NOBDW
		    env.stck = vec_at(env.memory, env.stck).next;
#else
		    env.stck = nextnode1(env.stck);
#endif
		}
		if (env.autoput)
		    putchar('\n');
	    }
	}
    }
einde:
    if (pstats)
	stats(&env);
    if (psdump)
	dump(&env);
    return 0;
}

int main(int argc, char **argv)
{
    int (*volatile m)(int, char **) = my_main;

    bottom_of_stack = (char *)&argc;
    GC_INIT();
    return (*m)(argc, argv);
}

/*
 * print statistics.
 */
void stats(pEnv env)
{
#ifdef NOBDW
    double perc;
#endif
    double diff;

    if ((diff = clock() - env->startclock) < 0)
	diff = 0;
    printf("%.0f milliseconds CPU to execute\n",
	   diff * 1000 / CLOCKS_PER_SEC);
#ifdef NOBDW
    if (diff && env->gc_clock) {
	perc = env->gc_clock * 100.0 / diff;
	printf("%.0f milliseconds CPU for gc (=%.0f%%)\n",
		env->gc_clock * 1000.0 / CLOCKS_PER_SEC, perc);
    }
#endif
    printf("%.0f nodes used\n", env->nodes);
#ifdef NOBDW
    printf("%.0f user nodes available\n", env->avail);
    printf("%.0f main garbage collections\n", env->collect);
#endif
    printf("%.0f garbage collections\n", (double)GC_get_gc_no());
    printf("%.0f calls to joy interpreter\n", env->calls);
    printf("%.0f operations executed\n", env->opers);
}

/*
 * dump the symbol table.
 */
void dump(pEnv env)
{
    int i;
    Entry ent;

    for (i = vec_size(env->symtab) - 1; i >= 0; i--) {
	ent = vec_at(env->symtab, i);
	if (!ent.is_user)
	    printf("(%d) %s\n", i, ent.name);
	else {
	    printf("(%d) %s == ", i, ent.name);
	    writeterm(env, ent.u.body, stdout);
	    putchar('\n');
	}
    }
}
