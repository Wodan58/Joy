/*
 *  module  : utils.c
 *  version : 1.19.1.12
 *  date    : 03/14/21
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "gc.h"
#include "globals.h"

static Index memoryindex, mem_low = 1, mem_mid, mem_high = MEMORYMAX - 1;
static int direction = 1, start_gc_clock;

#ifdef ENABLE_TRACEGC
static int nodesinspected, nodescopied;
#endif

PUBLIC void inimem1(pEnv env)
{
    env->stck = env->conts = env->dump = NULL;
    env->dump1 = env->dump2 = env->dump3 = env->dump4 = env->dump5 = NULL;
    direction = 1;
    memoryindex = mem_low;
}

#ifdef STATS
static double avail;

static void report_avail(void)
{
    fprintf(stderr, "%.0f user nodes available\n", avail);
}

static void count_avail(void)
{
    double new_avail;

    if (!avail)
        atexit(report_avail);
    new_avail = mem_high - mem_low + 1;
    if (avail > new_avail || !avail)
        avail = new_avail;
}
#endif

PUBLIC void inimem2(pEnv env)
{
    mem_low = memoryindex;
    mem_mid = mem_low + (mem_high - mem_low) / 2;

#ifdef STATS
    count_avail();
#endif
#ifdef ENABLE_TRACEGC
    if (tracegc > 1) {
        printf("memoryindex = %d\n", memoryindex);
        printf("mem_low = %d\n", mem_low);
        printf("top of mem = %d\n", mem_high);
        printf("mem_mid = %d\n", mem_mid);
    }
#endif
}

#ifdef ENABLE_TRACEGC
PUBLIC void printnode(pEnv env, Index p)
{
    printf("%10d:\t%-10s %10d %10d\n", p, vec_at(env->symtab, nodetype(p)).name,
        nodetype(p) == LIST_ ? nodevalue(p).lis : nodevalue(p).num,
        nextnode1(p));
}
#endif

/*
    Copy a single node from from_space to to_space.
*/
PRIVATE Index copyone(pEnv env, Index n)
{
#ifdef ENABLE_TRACEGC
    nodesinspected++;
    if (tracegc > 4)
        printf("copy ..\n");
#endif
    if (n < mem_low)
        return n;
    if (env->memory[n].op != COPIED_) {
        env->memory[memoryindex] = env->memory[n];
        env->memory[n].op = COPIED_;
        env->memory[n].u.lis = memoryindex;
#ifdef ENABLE_TRACEGC
        nodescopied++;
        if (tracegc > 3) {
            printf("%5d -    ", nodescopied);
            printnode(env, memoryindex);
        }
#endif
        memoryindex += direction;
    }
    return env->memory[n].u.lis;
}

/*
    Repeat copying single nodes until done.
*/
PRIVATE void copyall(pEnv env)
{
    Index scan;

    scan = direction == 1 ? mem_low : mem_high;
    for (; scan != memoryindex; scan += direction) {
        if (env->memory[scan].op == LIST_)
            env->memory[scan].u.lis = copyone(env, env->memory[scan].u.lis);
        env->memory[scan].next = copyone(env, env->memory[scan].next);
    }
}

#ifdef STATS
static double collect;

static void report_collect(void)
{
    fprintf(stderr, "%.0f garbage collections\n", collect);
}

static void count_collect(void)
{
    if (++collect == 1)
        atexit(report_collect);
}
#endif

#ifdef ENABLE_TRACEGC
PRIVATE void gc1(pEnv env, char *mess)
#else
PRIVATE void gc1(pEnv env)
#endif
{
    start_gc_clock = clock();
    direction = -direction;
    memoryindex = direction == 1 ? mem_low : mem_high;

#ifdef STATS
    count_collect();
#endif
#ifdef ENABLE_TRACEGC
    if (tracegc > 1)
        printf("begin %s garbage collection\n", mess);
    nodesinspected = nodescopied = 0;
#define COP(X, NAME)                                                           \
    do                                                                         \
        if (X) {                                                               \
            if (tracegc > 2) {                                                 \
                printf("old %s = ", NAME);                                     \
                writeterm(env, X, stdout);                                     \
                printf("\n");                                                  \
            }                                                                  \
            X = copyone(env, X);                                               \
            if (tracegc > 2) {                                                 \
                printf("new %s = ", NAME);                                     \
                writeterm(env, X, stdout);                                     \
                printf("\n");                                                  \
            }                                                                  \
        }                                                                      \
    while (0)
#else
#define COP(X, NAME) X = copyone(env, X)
#endif

    COP(env->stck, "stck");
    COP(env->prog, "prog");
    COP(env->conts, "conts");
    COP(env->dump, "dump");
    COP(env->dump1, "dump1");
    COP(env->dump2, "dump2");
    COP(env->dump3, "dump3");
    COP(env->dump4, "dump4");
    COP(env->dump5, "dump5");
}

#ifdef ENABLE_TRACEGC
PRIVATE void gc2(pEnv env, char *mess)
#else
PRIVATE void gc2(pEnv env)
#endif
{
    int this_gc_clock;

    copyall(env);
    this_gc_clock = clock() - start_gc_clock;
    if (this_gc_clock == 0)
        this_gc_clock = 1; /* correction */
    gc_clock += this_gc_clock;

#ifdef ENABLE_TRACEGC
    if (tracegc > 0)
        printf("gc - %d nodes inspected, %d nodes copied, clock: %d\n",
            nodesinspected, nodescopied, this_gc_clock);
    if (tracegc > 1)
        printf("end %s garbage collection\n", mess);
#endif
}

PUBLIC void gc_(pEnv env)
{
#ifdef ENABLE_TRACEGC
    gc1(env, "user requested");
    gc2(env, "user requested");
#else
    gc1(env);
    gc2(env);
#endif
}

#ifdef STATS
static double nodes;

static void report_nodes(void) { fprintf(stderr, "%.0f nodes used\n", nodes); }

static void count_nodes(void)
{
    if (++nodes == 1)
        atexit(report_nodes);
}
#endif

PUBLIC Index newnode(pEnv env, Operator o, Types u, Index r)
{
    Index p;

    if (memoryindex == mem_mid) {
#ifdef ENABLE_TRACEGC
        gc1(env, "automatic");
#else
        gc1(env);
#endif
        if (o == LIST_)
            u.lis = copyone(env, u.lis);
        r = copyone(env, r);
#ifdef ENABLE_TRACEGC
        gc2(env, "automatic");
#else
        gc2(env);
#endif
        if ((direction == 1 && memoryindex >= mem_mid)
            || (direction == -1 && memoryindex <= mem_mid))
            execerror(env, "memory", "copying");
    }
    p = memoryindex;
    memoryindex += direction;
    env->memory[p].op = o;
    env->memory[p].u = u;
    env->memory[p].next = r;
#ifdef STATS
    count_nodes();
#endif
    return p;
}

PUBLIC void memoryindex_(pEnv env)
{
    env->stck = INTEGER_NEWNODE(memoryindex, env->stck);
}

PUBLIC void memorymax_(pEnv env)
{
    env->stck = INTEGER_NEWNODE(mem_high + 1, env->stck);
}

PUBLIC void readfactor(pEnv env, int priv) /* read a JOY factor */
{
    long_t set = 0;
    pEntry mod_fields;
    Entry ent;

    switch (symb) {
    case ATOM:
        lookup(env, priv);
        while (location) {
            ent = vec_at(env->symtab, location);
            if (!ent.is_module)
                break;
            mod_fields = ent.u.module_fields;
            getsym(env);
            if (symb != PERIOD)
                error("period '.' expected after module name");
            else
                getsym(env);
            if (symb != ATOM) {
                error("atom expected as module field");
                return;
            }
            while (mod_fields) {
                ent = vec_at(env->symtab, mod_fields);
                if (!strcmp(ident, ent.name))
                    break;
                mod_fields = ent.next;
            }
            if (mod_fields == NULL) {
                error("no such field in module");
                abortexecution_(env);
            }
            location = mod_fields;
        }
        if (!priv) {
            if (location < firstlibra) {
                env->yylval.proc = vec_at(env->symtab, location).u.proc;
                env->stck = newnode(env, location, env->yylval, env->stck);
            } else
                env->stck = USR_NEWNODE(location, env->stck);
        }
        return;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        if (!priv)
            env->stck = newnode(env, symb, env->yylval, env->stck);
        return;
    case STRING_:
        if (!priv)
            env->stck = newnode(env, symb, env->yylval, env->stck);
        return;
    case FLOAT_:
        if (!priv)
            env->stck = FLOAT_NEWNODE(env->yylval.dbl, env->stck);
        return;
    case LBRACE:
        while (getsym(env), symb != RBRACE)
            if (symb == CHAR_ || symb == INTEGER_)
                set |= ((long_t)1 << env->yylval.num);
            else
                error("numeric expected in set");
        if (!priv)
            env->stck = SET_NEWNODE(set, env->stck);
        return;
    case LBRACK:
        getsym(env);
        readterm(env, priv);
        if (symb != RBRACK)
            error("']' expected");
        return;
    case LPAREN:
        error("'(' not implemented");
        getsym(env);
        return;
    default:
        error("a factor cannot begin with this symbol");
    }
}

PUBLIC void readterm(pEnv env, int priv)
{
    Index *my_dump = 0;

    if (!priv) {
        env->stck = LIST_NEWNODE(0, env->stck);
        my_dump = &nodevalue(env->stck).lis;
    }
    while (symb <= ATOM) {
        readfactor(env, priv);
        if (!priv) {
            *my_dump = env->stck;
            my_dump = &nextnode1(env->stck);
            env->stck = *my_dump;
            *my_dump = 0;
        }
        getsym(env);
    }
}

PRIVATE void my_writefactor(pEnv env, Node *n, FILE *stm)
{
    int i;
    char *p;
    long_t set;

    if (n == NULL)
        execerror(env, "non-empty stack", "print");
    switch (n->op) {
    case BOOLEAN_:
        fprintf(stm, "%s", n->u.num ? "true" : "false");
        return;
    case INTEGER_:
        fprintf(stm, "%ld", (long)n->u.num); /* BIT_32 */
        return;
    case FLOAT_:
        fprintf(stm, "%g", n->u.dbl);
        return;
    case SET_:
        set = n->u.set;
        fprintf(stm, "{");
        for (i = 0; i < SETSIZE; i++)
            if (set & ((long_t)1 << i)) {
                fprintf(stm, "%d", i);
                set &= ~((long_t)1 << i);
                if (set != 0)
                    fprintf(stm, " ");
            }
        fprintf(stm, "}");
        return;
    case CHAR_:
        fprintf(stm, "'%c", (char)n->u.num);
        return;
    case STRING_:
        fputc('"', stm);
        for (p = n->u.str; p && *p; p++) {
            if (*p == '"' || *p == '\\' || *p == '\n')
                fputc('\\', stm);
            fputc(*p == '\n' ? 'n' : *p, stm);
        }
        fputc('"', stm);
        return;
    case LIST_:
        fprintf(stm, "%s", "[");
        writeterm(env, n->u.lis, stm);
        fprintf(stm, "%s", "]");
        return;
    case USR_:
        fprintf(stm, "%s", vec_at(env->symtab, n->u.ent).name);
        return;
    case FILE_:
        if (n->u.fil == NULL)
            fprintf(stm, "file:NULL");
        else if (n->u.fil == stdin)
            fprintf(stm, "file:stdin");
        else if (n->u.fil == stdout)
            fprintf(stm, "file:stdout");
        else if (n->u.fil == stderr)
            fprintf(stm, "file:stderr");
        else
            fprintf(stm, "file:%p", (void *)n->u.fil);
        return;
    default:
        fprintf(stm, "%s", opername(n->op));
        return;
    }
}

PUBLIC void writefactor(pEnv env, Index n, FILE *stm)
{
    my_writefactor(env, &env->memory[n], stm);
}

PUBLIC void writeterm(pEnv env, Index n, FILE *stm)
{
    while (n != NULL) {
        my_writefactor(env, &env->memory[n], stm);
        n = nextnode1(n);
        if (n != NULL)
            fprintf(stm, " ");
    }
}
