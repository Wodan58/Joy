/* FILE: main.c */
/*
 *  module  : main.c
 *  version : 1.55
 *  date    : 07/19/23
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

#define ERROR_ON_USRLIB 0
#define DONT_READ_AHEAD 0
#define READ_PRIV_AHEAD 1

PRIVATE void dump_table(pEnv env);

static jmp_buf begin;

/*
 *   Initialise the symbol table with builtins. There is no need to classify
 *   builtins.
 */
PRIVATE void inisymboltable(pEnv env) /* initialise */
{
    int i, rv;
    Entry ent;
    khiter_t key;

    env->hash = kh_init(Symtab);
    for (i = 0; (ent.name = opername(i)) != 0; i++) {
        ent.is_user = 0;
        ent.u.proc = operproc(i);
        key = kh_put(Symtab, env->hash, ent.name, &rv);
        kh_value(env->hash, key) = i;
        vec_push(env->symtab, ent);
    }
}

/*
 *   Global identifiers are stored at location. The hash table uses a classified
 *   name. The name parameter has already been allocated.
 */
PRIVATE void enterglobal(pEnv env, char *name)
{
    int rv;
    Entry ent;
    khiter_t key;

    env->location = vec_size(env->symtab);
    ent.name = name;
    ent.is_user = 1;
    ent.u.body = 0; /* may be assigned in definition */
    key = kh_put(Symtab, env->hash, ent.name, &rv);
    kh_value(env->hash, key) = env->location;
    vec_push(env->symtab, ent);
}

/*
 *   Lookup first searches ident in the local symbol tables, if not found in the
 *   global symbol table, if still not found enters ident in the global table.
 */
PUBLIC void lookup(pEnv env)
{
    /*
     * Sequential search in the local tables is replaced by a search in the
     * hash table, where each entry receives a unique key built from module +
     * name, or section number + name, separated by a full stop. The hash table
     * thus contains local symbols, but each local section has its own numeric
     * identifier. Local symbols are only accessable from their accompanying
     * public sections. In case a qualified symbol cannot be found, it is
     * entered as classified symbol in the symbol table. Global symbols are not
     * added during the first time read of private sections.
     */
    if ((env->location = qualify(env, env->yylval.str)) == 0)
        /* not found, enter in global, unless it is a module-member  */
        if (strchr(env->yylval.str, '.') == 0)
            enterglobal(env, env->yylval.str);
}

/*
 *   Enteratom enters a symbol in the symbol table, maybe a local symbol. This
 *   local symbol is also added to the hash table, but in its classified form.
 */
PUBLIC void enteratom(pEnv env, int priv)
{
    /*
     *   Local symbols are only added during the first read of private sections
     *   and public sections of a module.
     *   They should be found during the second read.
     */
    if ((env->location = qualify(env, env->yylval.str)) == 0)
        enterglobal(env, classify(env, env->yylval.str));
}

/*
 *   The rest of these procedures is not affected by the change of the symbol
 *   table implementation.
 */
PRIVATE void defsequence(pEnv env, int priv);  /* forward */
PRIVATE void compound_def(pEnv env, int priv); /* forward */

/*
 *   Read a definition. Instead of a definition, an embedded compound definition
 *   is also possible.
 */
PRIVATE void definition(pEnv env, int priv)
{
    Entry ent;
    pEntry here = 0;

    if (env->symb == LIBRA || env->symb == JPRIVATE || env->symb == HIDE
        || env->symb == MODULE) {
        compound_def(env, priv);
        if (env->symb == END || env->symb == PERIOD)
            getsym(env);
        else
            error(env, " END or period '.' expected in compound definition");
        return;
    }

    if (env->symb != ATOM)
        /*   NOW ALLOW EMPTY DEFINITION:
              { error(env, "atom expected at start of definition");
                abortexecution_(); }
        */
        return;

    /* symb == ATOM : */
    if (!priv) {
        enteratom(env, priv);
        ent = vec_at(env->symtab, env->location);
        if (!ent.is_user) {
            fprintf(stderr, "warning: overwriting inbuilt '%s'\n", ent.name);
            enterglobal(env, classify(env, env->yylval.str));
        }
        here = env->location;
    }
    getsym(env);
    if (env->symb == EQDEF)
        getsym(env);
#if 0
    else
        error(env, " == expected in definition");
#endif
    readterm(env, priv);
    if (!priv && here && env->stck && nodetype(env->stck) == LIST_) {
        vec_at(env->symtab, here).u.body = nodevalue(env->stck).lis;
        env->stck = nextnode1(env->stck);
    }
}

/*
 *   defsequence - when reading the private section ahead, symbols are entered
 *                 in the symbol table, such that local symbols can call each
 *                 other.
 */
PRIVATE void defsequence(pEnv env, int priv)
{
    if (priv && env->symb == ATOM)
        enteratom(env, priv);
    definition(env, priv);
    while (env->symb == SEMICOL) {
        getsym(env);
        if (priv && env->symb == ATOM)
            enteratom(env, priv);
        definition(env, priv);
    }
}

/*
    In case of a HIDE section or a MODULE, some read ahead is necessary.
    Tokens are collected in a list. During second reading tokens are returned
    from the list instead of from stdin.
*/
#ifdef READ_PRIVATE_AHEAD
PRIVATE void read_priv_ahead(pEnv env)
{
    int token_index;

    env->token_list = 1; /* start collecting tokens */
    token_index = env->token_index;
    compound_def(env, READ_PRIV_AHEAD);
    env->token_index = token_index;
    env->token_list = 0; /* stop collecting tokens */
}
#endif

/*
    Handle a compound definition.
*/
PRIVATE void compound_def(pEnv env, int priv)
{
    switch (env->symb) {
    case MODULE:
        getsym(env);
        if (env->symb != ATOM) {
            error(env, "atom expected as name of module");
            abortexecution_();
        }
        initmod(env, env->yylval.str); /* initmod adds ident to the module */
        getsym(env);
#ifdef READ_PRIVATE_AHEAD
        if (env->symb == JPUBLIC) { /* MODULE with only a public section */
            ungetsym(JPUBLIC);
            env->symb = JPRIVATE;
        }
#endif
        compound_def(env, priv);
        exitmod(); /* exitmod deregisters a module */
        break;
    case JPRIVATE:
    case HIDE:
#ifdef READ_PRIVATE_AHEAD
        if (!priv)
            read_priv_ahead(env);
#endif
        getsym(env);
        initpriv(env, priv); /* initpriv increases the hide number */
        defsequence(env, priv);
        stoppriv(); /* stoppriv changes the section from private to public */
        compound_def(env, priv);
        exitpriv(); /* exitpriv lowers the hide stack */
        break;
    case JPUBLIC:
    case LIBRA:
    case IN:
        getsym(env);
        defsequence(env, priv);
        break;
    default:
#if 0
        fprintf(stdout, "warning: empty compound definition\n");
#endif
        break;
    }
}

/*
    abort execution and restart reading from srcfile. In the NOBDW version the
    stack is cleared as well.
*/
PUBLIC void abortexecution_(void)
{
    longjmp(begin, 1);
}

/*
    fatal terminates the program after a stack overflow, likely to result in
    heap corruption that makes it impossible to continue. And exit instead of
    _exit may fail too.
*/
#ifndef BDW
PRIVATE void fatal(void)
{
    fflush(stdout);
    fprintf(stderr, "fatal error: memory overflow\n");
    exit(EXIT_FAILURE);
}
#endif

/*
    print a runtime error to stderr and abort the execution of current program.
*/
PUBLIC void execerror(char *message, char *op)
{
    fflush(stdout);
    fprintf(stderr, "run time error: %s needed for %s\n", message, op);
    abortexecution_();
}

/*
    DUMP_CHECK - check that the dumps are not empty.
*/
#define DUMP_CHECK(D, NAME)                                                    \
    if (D) {                                                                   \
        printf("->  %s is not empty:\n", NAME);                                \
        writedump(&env, D);                                                    \
        putchar('\n');                                                         \
    }

/*
    report_clock - report the amount of time spent and in case of NOBDW also
                   report the time spent while doing garbage collection.
*/
#ifdef STATS
PRIVATE void report_clock(pEnv env)
{
    double timediff;
#ifdef NOBDW
    double gcdiff, gclock;
#endif

    timediff = clock() - env->startclock;
#ifdef NOBDW
    gcdiff = (double)env->gc_clock * 100 / timediff;
#endif
    timediff /= CLOCKS_PER_SEC;
    fprintf(stderr, "%.2f seconds CPU to execute\n", timediff);
#ifdef NOBDW
    gclock = (double)env->gc_clock / CLOCKS_PER_SEC;
    fprintf(stderr, "%.2f seconds CPU for gc (=%.0f%%)\n", gclock, gcdiff);
#endif
}
#endif

/*
 *   copyright - Print all copyright notices, even historical ones.
 *
 *   The version must be set on the commandline when compiling:
 *   -DJVERSION="\"alpha\"" or whatever.
 */
PRIVATE void copyright(char *file)
{
    int i, j = 0;
    char str[BUFFERMAX], *ptr;

    static struct {
        char *file;
        time_t stamp;
        char *gc;
    } table[] = {
        { "joytut.inp", 994075177, "NOBDW" },
        { "jp-joytst.joy", 994075177, "NOBDW" },
        { "laztst.joy", 1005579152, "BDW" },
        { "symtst.joy", 1012575285, "BDW" },
        { "plgtst.joy", 1012575285, "BDW" },
        { "lsptst.joy", 1012575285, "BDW" },
        { "mtrtst.joy", 1017847160, "BDW" },
        { "grmtst.joy", 1017847160, "BDW" },
        { "reptst.joy", 1047653638, "NOBDW" },
        { "jp-reprodtst.joy", 1047653638, "NOBDW" },
        { "flatjoy.joy", 1047653638, "NOBDW" },
        { "modtst.joy", 1047920271, "BDW" },
        { 0, 1056113062, "NOBDW" } };

    if (file) {
        if ((ptr = strrchr(file, '/')) != 0)
            file = ptr + 1;
        for (i = 0; table[i].file; i++) {
            if (!strcmp(file, table[i].file)) {
                strftime(str, sizeof(str), "%H:%M:%S on %b %e %Y",
                    gmtime(&table[i].stamp));
                printf("JOY  -  compiled at %s (%s)\n", str, table[i].gc);
                j = 1;
                break;
            }
        }
    } else {
        printf("JOY  -  compiled at %s on %s", __TIME__, __DATE__);
        printf(JVERSION ? " (%s)\n" : "\n", JVERSION);
        j = 1;
    }
    if (j)
        printf("Copyright 2001 by Manfred von Thun\n");
}

/*
    dump the symbol table - accessed from quit_, because env is needed;
    do this only for user defined symbols.
*/
PRIVATE void dump_table(pEnv env)
{
    int i;
    Entry ent;

    for (i = vec_size(env->symtab) - 1; i >= 0; i--) {
        ent = vec_at(env->symtab, i);
        if (!ent.is_user)
            printf("(%d) %s\n", i, ent.name);
        else {
            printf("(%d) %s == ", i, ent.name);
            writeterm(env, ent.u.body);
            putchar('\n');
        }
    }
}

/*
    options - print help on startup options and exit: options are those that
              cannot be set from within the language itself.
*/
PRIVATE void options(void)
{
    printf("Usage: joy [options] [filename] [parameters]\n");
    printf("options, filename, parameters can be given in any order\n");
    printf("options start with '-' and are all given together\n");
    printf("parameters start with a digit\n");
    printf("the filename parameter cannot start with '-' or a digit\n");
    printf("Options:\n");
    printf("  -h : print this help text and exit\n");
#ifdef TRACING
    printf("  -d : print a trace of program execution\n");
#endif
#ifdef SYMBOLS
    printf("  -s : dump user-defined functions after execution\n");
#endif
#ifdef COPYRIGHT
    printf("  -v : do not print a copyright notice\n");
#endif
    exit(0);
}

int start_main(int argc, char **argv)
{
    static unsigned char mustinclude = 1;
    int i, j;
    char *filename = 0, *ptr;
    unsigned char helping = 0;
#ifdef COPYRIGHT
    unsigned char verbose = 1;
#endif
#ifdef SYMBOLS
    unsigned char symdump = 0;
#endif

    Env env; /* memory, symbol table, stack, and buckets */
    memset(&env, 0, sizeof(env));

    /*
     *    Start the clock. my_atexit is called from quit_ that is called in
     *    scan.c after reading EOF on the first input file.
     */
    env.startclock = clock();
#ifdef STATS
    my_atexit(report_clock);
#endif
    /*
     *    Initialize srcfile and other environmental parameters.
     */
    env.srcfile = stdin;
    if ((ptr = strrchr(env.pathname = argv[0], '/')) != 0)
        *ptr = 0;
    else
        env.pathname = ".";
    /*
     *    First look for options. They start with -.
     */
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j]; j++)
                switch (argv[i][j]) {
                case 'h' : helping = 1; break;
#ifdef TRACING
                case 'd' : env.debugging = 1; break;
#endif
#ifdef SYMBOLS
                case 's' : symdump = 1; break;
#endif
#ifdef COPYRIGHT
                case 'v' : verbose = 0; break;
#endif
                }
            /*
                Overwrite the options with subsequent parameters.
            */
            for (--argc; i < argc; i++)
                argv[i] = argv[i + 1];
            break;
        }
    /*
     *    Look for a possible filename parameter. Filenames cannot start with -
     *    and cannot start with a digit, unless preceded by a path: e.g. './'.
     */
    for (i = 1; i < argc; i++)
        if (!isdigit(argv[i][0])) {
            if ((env.srcfile = freopen(filename = argv[i], "r", stdin)) == 0) {
                fprintf(stderr, "failed to open the file '%s'.\n", filename);
                return 0;
            }
            /*
             *   Overwrite argv[0] with the filename and shift subsequent
             *   parameters. Also change directory to that filename.
             */
            if ((ptr = strrchr(argv[0] = filename, '/')) != 0) {
                *ptr++ = 0;
                if (strcmp(filename, "."))
                    chdir(filename);
                argv[0] = filename = ptr;
            }
            for (--argc; i < argc; i++)
                argv[i] = argv[i + 1];
            break;
        }
    env.g_argc = argc;
    env.g_argv = argv;
#ifdef COPYRIGHT
    if (verbose)
        copyright(filename);
#endif
#ifdef SYMBOLS
    if (symdump)
        my_atexit(dump_table);
#endif
    if (helping)
        options();
    env.echoflag = INIECHOFLAG;
#ifdef NOBDW
    env.tracegc = INITRACEGC;
#endif
    env.autoput = INIAUTOPUT;
    env.undeferror = INIUNDEFERROR;
    inilinebuffer(&env, filename);
    inisymboltable(&env);
    setjmp(begin);
#ifdef NOBDW
    inimem1(&env);
    inimem2(&env);
#endif
    if (mustinclude) {
        mustinclude = 0;
        doinclude(&env, "usrlib.joy", ERROR_ON_USRLIB);
    }
    while (1) {
        getsym(&env);
        if (env.symb == LIBRA || env.symb == HIDE || env.symb == MODULE) {
#ifdef NOBDW
            inimem1(&env);
            env.stck = 0;
#endif
            compound_def(&env, DONT_READ_AHEAD);
#ifdef NOBDW
            inimem2(&env);
#endif
        } else {
            readterm(&env, DONT_READ_AHEAD);
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
                    writeterm(&env, env.stck);
                else if (env.autoput == 1) {
                    writefactor(&env, env.stck);
#ifdef NOBDW
                    env.stck = vec_at(env.memory, env.stck).next;
#else
                    env.stck = nextnode1(env.stck);
#endif
                }
                putchar('\n');
            }
        }
    }
}

int main(int argc, char **argv)
{
    int (*volatile m)(int, char **) = start_main;

#ifdef BDW
    GC_INIT();
#else
    GC_init(&argc, fatal);
#endif
    return (*m)(argc, argv);
}

