/* FILE: main.c */
/*
 *  module  : main.c
 *  version : 1.41
 *  date    : 04/12/22
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
#define ALLOC
#include "globals.h"

#define ERROR_ON_USRLIB 0
#define DONT_READ_AHEAD 0
#define READ_PRIV_AHEAD 1

static jmp_buf begin;

/*
 *   Initialise the symbol table with builtins. There is no need to classify
 *   builtins.
 */
PUBLIC void inisymboltable(pEnv env) /* initialise */
{
    int i, rv;
    Entry ent;
    khiter_t key;

#ifdef NOBDW
    env->memory = GC_malloc(MEMORYMAX * sizeof(Node));
#endif
    env->hash = kh_init(Symtab);
    for (i = 0; (ent.name = opername(i)) != 0; i++) {
        ent.u.proc = operproc(i);
        key = kh_put(Symtab, env->hash, ent.name, &rv);
        kh_value(env->hash, key) = i;
        vec_push(env->symtab, ent);
    }
    env->firstlibra = i;
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
    if ((env->location = qualify(env, env->ident)) == 0)
        /* not found, enter in global */
        enterglobal(env, GC_strdup(env->ident));
}

/*
 *   Enteratom enters a symbol in the symbol table, maybe a local symbol. This
 *   local symbol is also added to the hash table, but in its classified form.
 */
PRIVATE void enteratom(pEnv env, int priv)
{
    /*
     *   Local symbols are only added during the first read of private sections
     *   and public sections of a module.
     *   They should be found during the second read.
     */
    if (priv)
        enterglobal(env, classify(env, env->ident));
    else
        lookup(env);
}

/*
 *   The rest of these procedures is not affected by the change of the symbol
 *   table implementation.
 */
PRIVATE void defsequence(pEnv env, int priv); /* forward */
PRIVATE void compound_def(pEnv env, int priv); /* forward */

PRIVATE void definition(pEnv env, int priv)
{
    pEntry here = NULL;

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
        if (env->location < env->firstlibra) {
            printf("warning: overwriting inbuilt '%s'\n",
                vec_at(env->symtab, env->location).name);
            enterglobal(env, GC_strdup(env->ident));
        }
        here = env->location;
    }
    getsym(env);
    if (env->symb == EQDEF)
        getsym(env);
    else
        error(env, " == expected in definition");
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
    if (priv)
        enteratom(env, priv);
    definition(env, priv);
    while (env->symb == SEMICOL) {
        getsym(env);
        if (priv && env->symb == ATOM)
            enteratom(env, priv);
        definition(env, priv);
    }
}

PRIVATE void compound_def(pEnv env, int priv)
{
    int linenum;
    long offset;

    switch (env->symb) {
    case MODULE:
        getsym(env);
        if (env->symb != ATOM) {
            error(env, "atom expected as name of module");
            abortexecution_(env);
        }
        /* initmod adds ident to the module stack */
        initmod(env, env->ident);
        getsym(env);
        compound_def(env, priv);
        /* exitmod deregisters a module */
        exitmod();
        break;
    case JPRIVATE:
    case HIDE:
        if (!priv && !isatty(fileno(env->srcfile))) {
            if ((offset = ftell(env->srcfile)) < 0)
                execerror(env, "ftell", "HIDE");
            linenum = getlinenum();
            compound_def(env, READ_PRIV_AHEAD);
            if (fseek(env->srcfile, offset, 0))
                execerror(env, "fseek", "HIDE");
            resetlinebuffer(linenum);
        }
        getsym(env);
        /* initpriv adds the hide number to the hide stack */
        initpriv(env, priv);
        defsequence(env, priv);
        /* stoppriv registers the transition from private to public */
        stoppriv();
        compound_def(env, priv);
        /* exitpriv lowers the hide stack after reading the public section */
        exitpriv();
        break;
    case JPUBLIC:
    case LIBRA:
    case IN:
        getsym(env);
        defsequence(env, priv);
        break;
    default:
        printf("warning: empty compound definition\n");
        break;
    }
}

PUBLIC void abortexecution_(pEnv env)
{
#ifdef NOBDW
    env->conts = env->dump = NULL;
    env->dump1 = env->dump2 = env->dump3 = env->dump4 = env->dump5 = NULL;
#endif
    longjmp(begin, 0);
}

PUBLIC void execerror(pEnv env, char *message, char *op)
{
    printf("run time error: %s needed for %s\n", message, op);
    abortexecution_(env);
}

#define DUMP_CHECK(D, NAME)                                                    \
    if (D) {                                                                   \
        printf("->  %s is not empty:\n", NAME);                                \
        writeterm(&env, D, stdout);                                            \
        putchar('\n');                                                         \
    }

#ifdef STATS
PRIVATE void report_clock(void)
{
    double timediff;
#ifdef NOBDW
    double gcdiff, gclock;
#endif

    timediff = clock() - startclock;
#ifdef NOBDW
    gcdiff = (double)gc_clock * 100 / timediff;
#endif
    timediff /= CLOCKS_PER_SEC;
    fprintf(stderr, "%.2f seconds CPU to execute\n", timediff);
#ifdef NOBDW
    gclock = (double)gc_clock / CLOCKS_PER_SEC;
    fprintf(stderr, "%.2f seconds CPU for gc (=%.0f%%)\n", gclock, gcdiff);
#endif
}
#endif

/*
 *   copyright - Print all copyright notices, even the historical ones.
 *
 *   The version must be set on the commandline: -DJVERSION="\"alpha\""
 *   or whatever.
 */
PRIVATE void copyright(char *file)
{
    int i;
    char str[INPLINEMAX];

    static struct {
        char *file;
        time_t stamp;
        char *gc;
    } table[] = { { "jp-joytst.joy", 994075177, "NOBDW" },
        { "joytut.inp", 994075177, "NOBDW" },
        { "laztst.joy", 1005579152, "BDW" },
        { "symtst.joy", 1012575285, "BDW" },
        { "plgtst.joy", 1012575285, "BDW" },
        { "lsptst.joy", 1012575285, "BDW" },
        { "mtrtst.joy", 1017847160, "BDW" },
        { "grmtst.joy", 1017847160, "BDW" },
        { "reptst.joy", 1047653638, "NOBDW" },
        { "jp-reprodtst.joy", 1047653638, "NOBDW" },
        { "flatjoy.joy", 1047653638, "NOBDW" },
        { "modtst.joy", 1047920271, "BDW" }, { 0, 1056113062, "NOBDW" },
        { 0, 0, 0 } };

    if (file) {
        for (i = 0;; i++) {
            if (!table[i].file || !strcmp(file, table[i].file)) {
                strftime(str, sizeof(str), "%H:%M:%S on %b %e %Y",
                    gmtime(&table[i].stamp));
                printf("JOY  -  compiled at %s (%s)\n", str, table[i].gc);
                break;
            }
        }
    } else {
        printf("JOY  -  compiled at %s on %s", __TIME__, __DATE__);
        printf(JVERSION ? " (%s)\n" : "\n", JVERSION);
    }
    printf("Copyright 2001 by Manfred von Thun\n");
}

int start_main(int argc, char **argv)
{
    int i, j;
    char *filename = 0;
    unsigned char verbose = 0, mustinclude = 1;

    Env env; /* memory, symbol table, stack, and buckets */

    /*
     *    Start the clock. The clock is a global variable, because it is read
     *    during atexit(). Atexit is needed, because scan.c calls quit_ that
     *    calls exit when reading end of file.
     */
    startclock = clock();
#ifdef STATS
    atexit(report_clock);
#endif
    /*
     *    Initialize srcfile and other environmental parameters.
     */
    memset(&env, 0, sizeof(env));
    env.srcfile = stdin;
    /*
     *    First look for options. They start with -.
     */
    for (i = 0; i < argc; i++)
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j]; j++) {
                if (argv[i][j] == 'd')
                    env.debugging = 1;
                if (argv[i][j] == 'v')
                    verbose = 1;
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
     *    and cannot start with a digit, unless quoted.
     */
    for (i = 1; i < argc; i++)
        if (!isdigit(argv[i][0])) {
            if ((env.srcfile = freopen(filename = argv[i], "r", stdin)) == 0) {
                fprintf(stderr, "failed to open the file '%s'.\n", filename);
                return 0;
            }
            /*
             *   Overwrite argv[0] with the filename and shift subsequent
             *   parameters.
             */
            argv[0] = filename;
            for (--argc; i < argc; i++)
                argv[i] = argv[i + 1];
            break;
        }
    env.g_argc = argc;
    env.g_argv = argv;
    if (verbose)
        copyright(filename);
    env.autoput = INIAUTOPUT;
    env.echoflag = INIECHOFLAG;
    env.undeferror = INIUNDEFERROR;
    inilinebuffer(&env, filename);
    inisymboltable(&env);
    env.stck = NULL;
#ifdef NOBDW
    env.tracegc = INITRACEGC;
    gc_clock = 0;
    inimem1(&env);
    inimem2(&env);
#endif
    setjmp(begin);
    if (mustinclude) {
        mustinclude = 0;
        doinclude(&env, "usrlib.joy", ERROR_ON_USRLIB);
    }
    while (1) {
        getsym(&env);
        if (env.symb == LIBRA || env.symb == HIDE || env.symb == MODULE) {
#ifdef NOBDW
            inimem1(&env);
#endif
            compound_def(&env, DONT_READ_AHEAD);
#ifdef NOBDW
            inimem2(&env);
#endif
        } else {
            readterm(&env, DONT_READ_AHEAD);
#ifdef NOBDW
            if (env.stck && env.memory[env.stck].op == LIST_) {
                env.prog = env.memory[env.stck].u.lis;
                env.stck = env.memory[env.stck].next;
                env.conts = NULL;
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
                    env.stck = env.memory[env.stck].next;
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

#ifdef GC_BDW
    GC_INIT();
#else
    GC_init(&argc);
#endif
    return (*m)(argc, argv);
}
