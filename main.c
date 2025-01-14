/* FILE: main.c */
/*
 *  module  : main.c
 *  version : 1.109
 *  date    : 01/14/25
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

static jmp_buf begin;		/* restart with empty program */

char *bottom_of_stack;		/* needed in gc.c */

static void stats(pEnv env), dump(pEnv env);
#ifdef MALLOC_DEBUG
static void mem_free(pEnv env);
#endif

/*
 * abort execution and restart reading from srcfile; the stack is not cleared.
 */
void abortexecution_(int num)
{
    fflush(stdin);
    longjmp(begin, num);
}

/*
 * fatal terminates the application with an error message.
 */
#if defined(NOBDW) && defined(_MSC_VER)
void fatal(char *str)
{
    fflush(stdout);
    fprintf(stderr, "fatal error: %s\n", str);
    abortexecution_(ABORT_QUIT);
}
#endif

/*
 * banner - print the banner that was present in joy0; the banner is only
 *	    printed with the -v option.
 */
static void banner(void)
{
    printf("JOY  -  compiled at %s on %s", __TIME__, __DATE__);
#ifdef VERS
    printf(" (%s)", VERS);
#endif
    putchar('\n');
    fflush(stdout);
}

/*
 * options - print help on startup options and exit: options are those that
 *	     cannot be set from within the language itself.
 */
static void options(int verbose)
{
    if (!verbose)
	banner();
    printf("Copyright 2001 by Manfred von Thun\n");
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
#ifdef BYTECODE
    printf("  -b : compile a joy program to bytecode\n");
#endif
#ifdef COMPILER
    printf("  -c : compile joy source into C source\n");
#endif
    printf("  -d : print a trace of stack development\n");
    printf("  -e : set the echoflag (0-3)\n");
#ifdef BYTECODE
    printf("  -f : display a byte code file and exit\n");
#endif
    printf("  -g : set the __tracegc flag (0-6)\n");
    printf("  -h : print this help text and exit\n");
    printf("  -i : ignore impure imperative functions\n");
#ifdef BYTECODE
    printf("  -j : filename parameter is binary\n");
#endif
    printf("  -k : allow keyboard input in raw mode\n");
    printf("  -l : do not read usrlib.joy at startup\n");
#ifdef COMPILER
    printf("  -m : compile joy source into Roy format\n");
    printf("  -n : compile joy source into Soy format\n");
#endif
#ifdef BYTECODE
    printf("  -o : optimize bytecode with inlining\n");
#endif
    printf("  -p : print debug list of tokens\n");
#ifdef BYTECODE
    printf("  -q : quick const folding of bytecodes\n");
#endif
    printf("  -s : dump symbol table after execution\n");
    printf("  -t : print a trace of program execution\n");
    printf("  -u : set the undeferror flag (0,1)\n");
    printf("  -v : print a small banner at startup\n");
    printf("  -w : no warnings: overwriting, arities\n");
    printf("  -x : print statistics at end of program\n");
}

/*
 * unknown_opt - report unknown option and point out -h option.
 */
static void unknown_opt(char *exe, int ch)
{
    printf("Unknown option argument: \"-%c\"\n", ch);
    printf("More info with: \"%s -h\"\n", exe);
}

/*
 * Push an integer on the stack. The stack is communicated through a global
 * variable that is only used here.
 */
static pEnv tmp_env;

void do_push_int(int num)
{
    tmp_env->bucket.num = num;
    tmp_env->stck = newnode(tmp_env, INTEGER_, tmp_env->bucket, tmp_env->stck);
}

static void my_main(int argc, char **argv)
{
    static unsigned char psdump = 0, pstats = 0;
#ifdef BYTECODE
    static unsigned char joy = 1;	/* assume joy source code */
#endif
    Env env;				/* global variables */
    int i, j, ch;
    char *ptr, *tmp, *exe;
    unsigned char helping = 0, unknown = 0, mustinclude = 1, verbose = 0,
		  raw = 0;
#ifdef BYTECODE
    FILE *fp = 0;
    unsigned char listing = 0, lining = 0, quick = 0;
#endif

    memset(&env, 0, sizeof(env));
    /*
     * Start the clock.
     */
    env.startclock = clock();
    /*
     * store the directory where the Joy binary is located in the list of
     * pathnames to be used when including files. argv[0] is modified such
     * that it contains only the basename.
     */
    vec_init(env.pathnames);
    ptr = strrchr(argv[0], '/');
#ifdef WINDOWS
    if (!ptr)
	ptr = strrchr(argv[0], '\\');
#endif
    if (ptr) {
	vec_push(env.pathnames, argv[0]);
	*ptr++ = 0;	/* split in directory */
	argv[0] = ptr;	/* and basename */
    }
    exe = argv[0];	/* Joy binary */
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
			   env.autoput = strtol(ptr, &tmp, 0);
			   j += tmp - ptr;
			   env.autoput_set = 1;		/* disable usrlib.joy */
			   break;
#ifdef BYTECODE
		case 'b' : env.bytecoding = -1; break;	/* prepare & suspend */
#endif
#ifdef COMPILER
		case 'c' : env.compiling = -1; break;	/* compile for joy1 */
#endif
		case 'd' : env.debugging = 1; break;
		case 'e' : ptr = &argv[i][j + 1];
			   env.echoflag = strtol(ptr, &tmp, 0);
			   j += tmp - ptr;
			   break;
#ifdef BYTECODE
		case 'f' : listing = 1; joy = 0; break;
#endif
		case 'g' : ptr = &argv[i][j + 1];
			   env.tracegc = strtol(ptr, &tmp, 0);
			   j += tmp - ptr;
			   break;
		case 'h' : helping = 1; break;
		case 'i' : env.ignore = 1; break;
#ifdef BYTECODE
		case 'j' : joy = 0; break;		/* assume binary file */
#endif
		case 'k' : raw = 1; break;		/* terminal raw mode */
		case 'l' : mustinclude = 0; break;
#ifdef COMPILER
		case 'm' : env.compiling = -2; break;	/* compile for Roy */
		case 'n' : env.compiling = -3; break;	/* compile for Soy */
#endif
#ifdef BYTECODE
		case 'o' : lining = 1; joy = 0; break;	/* inlining */
#endif
		case 'p' : env.printing = 1; break;
#ifdef BYTECODE
		case 'q' : quick = 1; joy = 0; break;	/* const folding */
#endif
		case 's' : psdump = 1; break;
		case 't' : env.debugging = 2; break;
		case 'u' : ptr = &argv[i][j + 1];
			   env.undeferror = strtol(ptr, &tmp, 0);
			   j += tmp - ptr;
			   env.undeferror_set = 1;	/* disable usrlib.joy */
			   break;
		case 'v' : verbose = 1; break;
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
     * Handle the banner now, before a possible error message is generated.
     */
    if (verbose)
	banner();
    /*
     * Look for a possible filename parameter. Filenames cannot start with -
     * and cannot start with a digit, unless preceded by a path: e.g. './'.
     */
    for (i = 1; i < argc; i++) {
	ch = argv[i][0];
	if (!isdigit(ch)) {
	    /*
	     * The first file should also benefit from include logic.
	     */
#ifdef BYTECODE
	    if (!joy) {
		if ((fp = fopen(env.filename = argv[i], "rb")) == 0) {
		    fprintf(stderr, "failed to open the file '%s'.\n", argv[i]);
		    return;
		}
	    } else
#endif
	    if (include(&env, env.filename = argv[i])) {
		fprintf(stderr, "failed to open the file '%s'.\n", argv[i]);
		return;
	    }
	    /*
	     *  Overwrite argv[0] with the filename and shift subsequent
	     *  parameters.
	     */
	    if ((ptr = strrchr(argv[0] = argv[i], '/')) != 0) {
		*ptr++ = 0;
		argv[0] = ptr;		/* Joy program basename */
	    }
	    for (--argc; i < argc; i++)
		argv[i] = argv[i + 1];
	    goto start;			/* only one filename; replaces stdin */
	} /* end if */
    } /* end for */
    inilinebuffer(&env);		/* initialize with stdin */
start:    
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
     * initialize stack before SetRaw.
     */
#ifdef NOBDW
    inimem1(&env, 0);		/* does not clear the stack */
    inimem2(&env);
#endif
    /*
     * initialize standard output.
     */
    if (raw && env.filename) {	/* raw requires a filename */
	env.autoput = 0;	/* disable autoput in usrlib.joy */
	env.autoput_set = 1;	/* prevent enabling autoput */
	tmp_env = &env;
	SetRaw();		/* keep output buffered */
	tmp_env = 0;
#ifdef NOBDW
	env.inits = env.stck;	/* remember initial stack */
	inimem2(&env);		/* store initial stack in definition space */
#endif
    } else
	setbuf(stdout, 0);	/* disable output buffering (pipe) */
    /*
     * read initial library.
     */
    if (mustinclude)
	include(&env, "usrlib.joy");
#ifdef BYTECODE
    if (quick) {
	compeval(&env, fp);	/* create .buc file, const folding */
	goto einde;
    }
    if (lining) {
	optimize(&env, fp);	/* create .boc file, inlining */
	goto einde;
    }
    if (listing) {
	dumpbyte(&env, fp);	/* display .bic, .boc, or .buc file */
	goto einde;
    }
    if (env.bytecoding && joy)
	initbytes(&env);	/* create .bic file from joy source */
#endif
#ifdef COMPILER
    if (env.compiling)
	initcompile(&env);	/* create .c file */
#endif
    /*
     * handle options, might print symbol table.
     */
    if (helping || unknown) {
	helping ? options(verbose) : unknown_opt(exe, unknown);
	goto einde;
    }
    /*
     * setup return address of error, abort, or quit.
     */
    if (setjmp(begin) == ABORT_QUIT)
	goto einde;		/* return here after error or abort */
    /*
     * (re)initialize code.
     */
    env.prog = 0;		/* clear program, just to be sure */
#ifdef BYTECODE
    if (!joy) {			/* interprete or compile bytecode */
	readbyte(&env, fp);
#ifdef COMPILER
	if (env.compiling) {	/* compile one program */
	    env.compiling = -env.compiling;
	    compile(&env, env.prog);	/* compile to C code */
	} else {
#endif
	    exeterm(&env, env.prog);	/* execute main program */
	    print(&env);	/* print result */
#ifdef COMPILER
	}
#endif
    } else
#endif
    repl(&env);			/* read-eval-print loop */
einde:
#ifdef BYTECODE
    if (env.bytecoding)
	exitbytes(&env);
#endif
#ifdef COMPILER
    if (env.compiling)
	exitcompile(&env);
#endif
    if (pstats)
	stats(&env);
    if (psdump)
	dump(&env);
#ifdef MALLOC_DEBUG
    mem_free(&env);
#endif
    SetNormal();		/* set the terminal back to normal */
}

int main(int argc, char **argv)
{
    void (*volatile m)(int, char **) = my_main;

    bottom_of_stack = (char *)&argc;
    GC_INIT();
    (*m)(argc, argv);
    return 0;
}

/*
 * print statistics.
 */
static void stats(pEnv env)
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
static void dump(pEnv env)
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

#ifdef MALLOC_DEBUG
static void mem_free(pEnv env)
{
    int i, j;
    Entry ent;

    /*
     * The strings in the symbol table have been moved to permanent memory.
     * They need to be released explicitly.
     */
    for (i = tablesize(), j = vec_size(env->symtab); i < j; i++) {
	ent = vec_at(env->symtab, i);
	free(ent.name);
    }
#ifdef NOBDW
    free(env->memory);
#endif
    kh_destroy(Symtab, env->hash);
    kh_destroy(Funtab, env->prim);
}
#endif
