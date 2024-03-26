/*
 *  module  : utils.c
 *  version : 1.30
 *  date    : 03/23/24
 */
#include "globals.h"

#define LOWER_LIMIT	  20000		/* minimum number of nodes */
#define UPPER_LIMIT	2000000		/* maximum number of nodes */

static clock_t start_gc_clock;
static vector(Node) *orig_memory;
static Index memoryindex, mem_low = 1;
static uint64_t memorymax = LOWER_LIMIT;

#ifdef TRACEGC
static int nodesinspected, nodescopied;
#endif

/*
 * Initialize memory at the start and before reading a definition.
 * Definitions clear all other memory; they are themselves permanent.
 * Memory is initialized with 1 node, acting as a null pointer.
 * The flag status tells whether a definition is about to be processed.
 */
void inimem1(pEnv env, int status)
{
    static unsigned char init;
    Node node;

    if (!init) {
	init = 1;
	memset(&node, 0, sizeof(Node));
	vec_init(env->memory);			/* initialize memory */
	vec_push(env->memory, node);		/* null pointer */
    } else if (status) {
	vec_setsize(env->memory, mem_low);	/* retain only definitions */
	env->stck = 0;				/* also clear the stack */
    }
    env->conts = env->dump = env->dump1 = env->dump2 =
    env->dump3 = env->dump4 = env->dump5 = 0;
}

/*
 * Reset mem_low after reading a definition. This invalidates all allocations
 * higher than mem_low: program, stack, dumps.
 */
void inimem2(pEnv env)
{
    double new_avail;

    mem_low = vec_size(env->memory);		/* enlarge definition space */
    new_avail = memorymax - mem_low;
    if (env->avail > new_avail || !env->avail)
	env->avail = new_avail;
#ifdef TRACEGC
    if (env->tracegc > 1) {
	printf("mem_low = %d\n", mem_low);
	printf("memoryindex = %d\n", memoryindex);
	printf("top of mem = %" PRId64 "\n", memorymax);
    }
#endif
}

#ifdef TRACEGC
void printnode(pEnv env, Index p)
{
    printf("%d: ", p);
    writefactor(env, p, stdout);
    printf(" :%d\n", nextnode1(p));

}
#endif

/*
 * Copy a single node from from_space to to_space.
 */
Index copyone(pEnv env, Index n)
{
#ifdef TRACEGC
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
#ifdef TRACEGC
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
 * Repeat copying single nodes until done.
 */
void copyall(pEnv env)
{
    Index scan;

    for (scan = mem_low; scan < memoryindex; scan++) {
	if (vec_at(env->memory, scan).op == LIST_)
	    vec_at(env->memory, scan).u.lis =
		   copyone(env, vec_at(env->memory, scan).u.lis);
	vec_at(env->memory, scan).next =
	       copyone(env, vec_at(env->memory, scan).next);
    }
    vec_setsize(env->memory, memoryindex);
    orig_memory = 0;
/*
 * Occupancy should be between 70% and 80%. If occupancy drops below 40%,
 * then the next maximum size can be set at 80% of what it was.
 * If occupancy is more than 80%, the next maximum is doubled, up to a limit.
 * If, after garbage collection, the number of live nodes equals UPPER_LIMIT
 * an error is generated. The UPPER_LIMIT prevents an ever increasing heap.
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

#ifdef TRACEGC
void gc1(pEnv env, char *mess)
#else
void gc1(pEnv env)
#endif
{
    start_gc_clock = clock();
/*
 * Copying the memory before garbage collecting seems a bit cautious. What
 * must be copied are the definitions at the start of the memory. The rest
 * could be added during garbage collection.
 *
 * copy at least 0 .. mem_low: vec_copy was replaced by vec_copy_count.
 */
    vec_copy_count(orig_memory, env->memory, vec_size(env->memory));
    memoryindex = mem_low;		/* start allocating from here */

    env->collect++;
#ifdef TRACEGC
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

#ifdef TRACEGC
void gc2(pEnv env, char *mess)
#else
void gc2(pEnv env)
#endif
{
    clock_t this_gc_clock;

    copyall(env);
    this_gc_clock = clock() - start_gc_clock;
    env->gc_clock += this_gc_clock;

#ifdef TRACEGC
    if (env->tracegc > 0)
	printf("gc - %d nodes inspected, %d nodes copied, clock: %ld\n",
	    nodesinspected, nodescopied, this_gc_clock);
    if (env->tracegc > 1)
	printf("end %s garbage collection\n", mess);
#endif
}

void my_gc(pEnv env)
{
#ifdef TRACEGC
    gc1(env, "user requested");
    gc2(env, "user requested");
#else
    gc1(env);
    gc2(env);
#endif
}

/*
 * newnode - allocate a new node or error out if no nodes are available.
 */
Index newnode(pEnv env, Operator o, Types u, Index r)
{
    Index p;
    Node node;

    if (vec_size(env->memory) == memorymax) {
#ifdef TRACEGC
	gc1(env, "automatic");
#else
	gc1(env);
#endif
	if (o == LIST_)
	    u.lis = copyone(env, u.lis);
	r = copyone(env, r);
#ifdef TRACEGC
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
    env->nodes++;
    return p;
}

/*
 * report the number of nodes that are currently used.
 */
void my_memoryindex(pEnv env)
{
    env->bucket.num = vec_size(env->memory);
    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
}

/*
 * report the number of nodes available.
 */
void my_memorymax(pEnv env)
{
    env->bucket.num = vec_max(env->memory);
    env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
}
