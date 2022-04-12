/*
 *  module  : utils.c
 *  version : 1.4
 *  date    : 04/12/22
 */
#include "globals.h"

static Index memoryindex, mem_low = 1, mem_mid, mem_high = MEMORYMAX - 1;
static int direction = 1;
static clock_t start_gc_clock;

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
    new_avail = mem_high - mem_low;
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
    if (env->tracegc > 1) {
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
    if (env->tracegc > 4)
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
        if (env->tracegc > 3) {
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
PUBLIC void gc1(pEnv env, char *mess)
#else
PUBLIC void gc1(pEnv env)
#endif
{
    start_gc_clock = clock();
    direction = -direction;
    memoryindex = direction == 1 ? mem_low : mem_high;

#ifdef STATS
    count_collect();
#endif
#ifdef ENABLE_TRACEGC
    if (env->tracegc > 1)
        printf("begin %s garbage collection\n", mess);
    nodesinspected = nodescopied = 0;
#define COP(X, NAME)                                                           \
    do                                                                         \
        if (X) {                                                               \
            if (env->tracegc > 2) {                                            \
                printf("old %s = ", NAME);                                     \
                writeterm(env, X, stdout);                                     \
                printf("\n");                                                  \
            }                                                                  \
            X = copyone(env, X);                                               \
            if (env->tracegc > 2) {                                            \
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
PUBLIC void gc2(pEnv env, char *mess)
#else
PUBLIC void gc2(pEnv env)
#endif
{
    clock_t this_gc_clock;

    copyall(env);
    this_gc_clock = clock() - start_gc_clock;
    gc_clock += this_gc_clock;

#ifdef ENABLE_TRACEGC
    if (env->tracegc > 0)
        printf("gc - %d nodes inspected, %d nodes copied, clock: %d\n",
            nodesinspected, nodescopied, this_gc_clock);
    if (env->tracegc > 1)
        printf("end %s garbage collection\n", mess);
#endif
}

PUBLIC void my_gc(pEnv env)
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

/*
    newnode - allocate a new node or error out if no nodes are available.
*/
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

PUBLIC void my_memoryindex(pEnv env)
{
    env->bucket.num = memoryindex;
    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
}

PUBLIC void my_memorymax(pEnv env)
{
    env->bucket.num = mem_high - mem_low;
    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
}

/*
    readterm - read a term from srcfile and push this on the stack as a list.
*/
PUBLIC void readterm(pEnv env, int priv)
{
    if (!priv) {
        env->bucket.lis = 0;
        env->stck = newnode(env, LIST_, env->bucket, env->stck);
    }
    if (env->symb <= ATOM) {
        readfactor(env, priv);
        if (!priv) {
            nodevalue(nextnode1(env->stck)).lis = env->stck;
            env->stck = nextnode1(env->stck);
            nextnode1(nodevalue(env->stck).lis) = 0;
            env->bucket.lis = nodevalue(env->stck).lis;
            env->dump = newnode(env, LIST_, env->bucket, env->dump);
        }
        while (getsym(env), env->symb <= ATOM) {
            readfactor(env, priv);
            if (!priv) {
                nextnode1(nodevalue(env->dump).lis) = env->stck;
                env->stck = nextnode1(env->stck);
                nextnode2(nodevalue(env->dump).lis) = 0;
                nodevalue(env->dump).lis = nextnode1(nodevalue(env->dump).lis);
            }
        }
        if (!priv)
            env->dump = nextnode1(env->dump);
    }
}

/*
    my_writefactor - like writefactor but with Index instead of Node *.
*/
PUBLIC void my_writefactor(pEnv env, Index n, FILE *stm)
{
    writefactor(env, &env->memory[n], stm);
}

/*
    writeterm - print the contents of a list in readable format.
*/
PUBLIC void writeterm(pEnv env, Index n, FILE *stm)
{
    while (n) {
        writefactor(env, &env->memory[n], stm);
        n = nextnode1(n);
        if (n)
            fputc(' ', stm);
    }
}
