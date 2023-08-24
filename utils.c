/*
 *  module  : utils.c
 *  version : 1.20
 *  date    : 08/24/23
 */
#include "globals.h"

#define LOWER_LIMIT	20000		/* minimum number of nodes */
#define UPPER_LIMIT	3000000		/* maximum number of nodes */

static clock_t start_gc_clock;
static vector(Node) *orig_memory;
static Index memoryindex, mem_low = 1;
static uint64_t memorymax = LOWER_LIMIT;

#ifdef ENABLE_TRACEGC
static int nodesinspected, nodescopied;
#endif

/*
    Initialize memory at the start and before reading a definition.
    Definitions clear all other memory; they are themselves permanent.
    Memory is initialized with 1 node, acting as a null pointer.
    The flag status tells whether a definition is about to be processed.
*/
PUBLIC void inimem1(pEnv env, int status)
{
    static unsigned char init;
    Node node;

    if (!init) {
	init = 1;
	memset(&node, 0, sizeof(Node));
	vec_init(env->memory);
	vec_push(env->memory, node); /* null pointer */
    } else if (status) {
	vec_setsize(env->memory, mem_low); /* retain only definitions */
	env->stck = 0; /* also clear the stack */
    }
    env->conts = env->dump = env->dump1 = env->dump2 =
    env->dump3 = env->dump4 = env->dump5 = 0;
}

#ifdef STATS
static double avail;

static void report_avail(void)
{
    fflush(stdout);
    fprintf(stderr, "%.0f user nodes available\n", avail);
}

static void count_avail(void)
{
    double new_avail;

    if (!avail)
	atexit(report_avail);
    new_avail = memorymax - mem_low;
    if (avail > new_avail || !avail)
	avail = new_avail;
}
#endif

/*
    Reset mem_low after reading a definition. This invalidates all allocations
    higher than mem_low: program, stack, dumps.
*/
PUBLIC void inimem2(pEnv env)
{
    mem_low = vec_size(env->memory); /* enlarge definition space */

#ifdef STATS
    count_avail();
#endif
#ifdef ENABLE_TRACEGC
    if (env->tracegc > 1) {
	printf("mem_low = %d\n", mem_low);
	printf("memoryindex = %d\n", memoryindex);
	printf("top of mem = %u\n", memorymax);
    }
#endif
}

#ifdef ENABLE_TRACEGC
PUBLIC void printnode(pEnv env, Index p)
{
    printf("%5d: %10s ", p, vec_at(env->symtab, nodetype(p)).name);
    if (nodetype(p) == USR_)
	printf("%10s", vec_at(env->symtab, nodevalue(p).num).name);
    else
	printf("%10d", nodevalue(p).num);
    printf(" %5d\n", nextnode1(p));
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
	printf("copy .. (%d)\n", n);
#endif
    if (n < mem_low)
	return n;
    if (vec_at(orig_memory, n).op != COPIED_) {
	vec_at(env->memory, memoryindex) = vec_at(orig_memory, n);
	vec_at(orig_memory, n).op = COPIED_;
	vec_at(orig_memory, n).u.lis = memoryindex;
#ifdef ENABLE_TRACEGC
	nodescopied++;
	if (env->tracegc > 3) {
	    printf("%5d -    ", nodescopied);
	    printnode(env, memoryindex);
	}
#endif
	memoryindex++;
    }
    return vec_at(orig_memory, n).u.lis;
}

/*
    Repeat copying single nodes until done.
*/
PRIVATE void copyall(pEnv env)
{
    Index scan;

    for (scan = mem_low; scan < memoryindex; scan++) {
	if (vec_at(env->memory, scan).op == LIST_)
	    vec_at(env->memory, scan).u.lis =
		   copyone(env, vec_at(env->memory, scan).u.lis);
	vec_at(env->memory, scan).next =
	       copyone(env, vec_at(env->memory, scan).next);
    }
    vec_setsize(orig_memory, 0);
    vec_setsize(env->memory, memoryindex);
/*
    Occupancy should be between 70% and 80%. If occupancy drops below 40%,
    then the next maximum size can be set at 80% of what it was.
    If occupancy is more than 80%, the next maximum is doubled, up to a limit.
    If, after garbage collection, the number of live nodes exceeds UPPER_LIMIT,
    an error is generated. This should prevent an ever increasing heap.
*/
    if (memoryindex * 100.0 / memorymax < 40) {		/* less than 40% */
	memorymax *= 0.8;				/* decrease memory */
	if (memorymax < LOWER_LIMIT)			/* up to a limit */
	    memorymax = LOWER_LIMIT;
    } else if (memoryindex * 100.0 / memorymax > 80) {	/* more than 80% */
	memorymax *= 2;					/* increase memory */
	if (memorymax > UPPER_LIMIT)			/* up to a limit */
	    memorymax = UPPER_LIMIT;
    }
}

#ifdef STATS
static double collect;

static void report_collect(void)
{
    fflush(stdout);
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
    vec_copy(orig_memory, env->memory);
    memoryindex = mem_low;

#ifdef STATS
    count_collect();
#endif
#ifdef ENABLE_TRACEGC
    if (env->tracegc > 1)
	printf("begin %s garbage collection\n", mess);
    nodesinspected = nodescopied = 0;
#endif

#define COP(X) X = copyone(env, X)

    COP(env->stck);
    COP(env->prog);
    COP(env->conts);
    COP(env->dump);
    COP(env->dump1);
    COP(env->dump2);
    COP(env->dump3);
    COP(env->dump4);
    COP(env->dump5);
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
    env->gc_clock += this_gc_clock;

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

static void report_nodes(void)
{
    fflush(stdout);
    fprintf(stderr, "%.0f nodes used\n", nodes);
}

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
    Node node;

    if (vec_size(env->memory) == memorymax) {
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
	if (vec_size(env->memory) == UPPER_LIMIT)
	    execerror("memory", "copying");
    }
    memset(&node, 0, sizeof(Node));
    node.u = u;
    node.op = o;
    node.next = r;
    p = vec_size(env->memory);
    vec_push(env->memory, node);
#ifdef STATS
    count_nodes();
#endif
    return p;
}

/*
    report the number of nodes that are currently used.
*/
PUBLIC void my_memoryindex(pEnv env)
{
    env->bucket.num = vec_size(env->memory);
    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
}

/*
    report the number of nodes available.
*/
PUBLIC void my_memorymax(pEnv env)
{
    env->bucket.num = vec_max(env->memory);
    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
}
