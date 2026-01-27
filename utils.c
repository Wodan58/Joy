/*
 *  module  : utils.c
 *  version : 1.56
 *  date    : 01/26/26
 */
#include "globals.h"

/*
 * Tracegc is normally turned off. It can be turned on, in case problems are
 * suspected. One aim of this file is to make it more flexible than the
 * original. Another aim is to not make it slower. Yet another aim is to not
 * use more memory than necessary. Large memory with fewer garbage collections
 * is not always faster than small memory with more collections.
 */
#if 0
#define TRACEGC
#endif

/*
 * MEM_LOW should allow reading usrlib.joy, inilib.joy, prelib.joy, and
 * agglib.joy without reallocation and without garbage collection. The
 * allocation is capped at INT_MAX nodes. That is 32 GiB and that should
 * be enough, for the time being. MEM_LOW is now "optimized" for fib(35).
 */
#define MEM_LOW		2000	/* initial number of nodes */

static clock_t start_gc_clock;	/* statistics */
#ifdef TRACEGC
static int nodesinspected, nodescopied;
#endif

char *check_strdup(char *str)
{
    char *ptr = strdup(str);
#ifdef TEST_MALLOC_RETURN
    if (!ptr)
	fatal("memory exhausted");
#endif
    return ptr;
}

void *check_malloc(size_t leng)
{
    void *ptr = malloc(leng);
#ifdef TEST_MALLOC_RETURN
    if (!ptr)
	fatal("memory exhausted");
#endif
    return ptr;
}

/*
 * Initialize memory at the start and before reading a definition.
 * Definitions clear all other memory; they are themselves permanent.
 * Memory is initialized with 1 node, acting as a null pointer.
 * The flag status tells whether a definition is about to be processed.
 */
void inimem1(pEnv env, int status)
{
#ifdef TRACEGC
    env->tracegc = 4;			/* set to reasonable, max=6 */
#endif
    if (!env->mem_low) {
	env->memoryindex = env->mem_low = 1;
	env->memory = calloc(env->memorymax = MEM_LOW, sizeof(Node));
#ifdef TEST_MALLOC_RETURN
	if (!env->memory)
	    fatal("memory exhausted");
#endif
    } else if (status) {
	env->stck = env->inits;		/* reset the stack to initial */
	env->memoryindex = env->mem_low;/* retain only definitions */
    }
    env->conts = env->dump = 0;
    env->dump1 = env->dump2 = env->dump3 = env->dump4 = env->dump5 = 0;
    env->flibrary_busy = 1;		/* disable garbage collection */
}

/*
 * Reset mem_low after reading a definition. This invalidates all allocations
 * higher than mem_low: program, stack, dumps.
 */
void inimem2(pEnv env)
{
    double new_avail;

    env->mem_low = env->memoryindex;	/* enlarge definition space */
    new_avail = env->memorymax - env->mem_low;
    if (env->avail > new_avail || !env->avail)
	env->avail = new_avail;
    env->flibrary_busy = 0;		/* enable garbage collection */
#ifdef TRACEGC
    if (env->tracegc > 1) {
	printf("mem_low = %u\n", env->mem_low);
	printf("memoryindex = %u\n", env->memoryindex);
	printf("top of mem = %lu\n", (unsigned long)env->memorymax);
    }
#endif
}

#ifdef TRACEGC
void printnode(pEnv env, Index p)
{
    int op;
    char *str;

    if ((op = nodetype(p)) == USR_)
	str = vec_at(env->symtab, nodevalue(p).ent).name;
    else if (op == ANON_FUNCT_)
	str = nickname(operindex(env, nodevalue(p).proc));
    else
	str = opername(op);
    printf("%d: %s %" PRId64 " %d\n", p, str, nodevalue(p).num, nextnode1(p));
}
#endif

/*
 * Copy a single node from from_space to to_space. The to_space should be as
 * large as the from_space or larger.
 */
static Index copy(pEnv env, Index n)
{
    Index temp;			/* index in to_space, te be returned */
    Operator op;
    int size, leng, num = 1;	/* number of physical nodes */

#ifdef TRACEGC
    nodesinspected++;
    if (env->tracegc > 4)
	printf("copy .. (%d)\n", n);
#endif
/*
 * If n is 0 or in the space reserved for definitions, it is returned
 * unmodified. Definitions have already been copied.
 */
    if (n < env->mem_low)
	return n;
/*
 * If the node has already been copied, then the new index is present in the
 * lis member.
 */
    if ((op = env->old_memory[n].op) == COPIED_)
	return env->old_memory[n].u.lis;
/*
 * Copy the node from the original to the new location. What gets copied is:
 * op, len, next, u.
 */
    env->memory[temp = env->memoryindex++] = env->old_memory[n];
/*
 * If the node contains a string, then some more copying is needed.
 */
    if (op == STRING_ || op == BIGNUM_) {
	size = leng = env->old_memory[n].len + 1;
	memcpy(&env->memory[temp].u, &env->old_memory[n].u, leng);
	if ((size -= sizeof(Types)) > 0)	/* first part in Types */
	    num += (size + sizeof(Node) - 1) / sizeof(Node);	/* round up */
	env->memoryindex += num;
    }
/*
 * If the node contains a list, then the list needs to be copied. This requires
 * that copy is called recursively.
 */
    if (op == LIST_)
	env->memory[temp].u.lis = copy(env, env->old_memory[n].u.lis);
/*
 * The next field also needs to be copied separately, overruling what was there
 * copied earlier on.
 */
    env->memory[temp].next = copy(env, env->old_memory[n].next);
/*
 * The original location is set to COPIED_: it will not be copied again.
 * The lis field is given the index in the to_space.
 */
    env->old_memory[n].op = COPIED_;
    env->old_memory[n].u.lis = temp;
#ifdef TRACEGC
    nodescopied++;
#endif
    return temp;
}

/*
 * Scan the symbol table for roots. Variables are not allocated in the space
 * that definitions occupy and therefore need to be taken care of separately.
 */
static void scan_roots(pEnv env)
{
    int i;
    Entry ent;

    /*
     * Look in the symbol table for variables. They also need to be garbage
     * collected.
     */
    for (i = vec_size(env->symtab) - 1; i > 0; i--) {
	ent = vec_at(env->symtab, i);
	if (!ent.is_user)		/* stop at builtins */
	    break;
	if (ent.is_root && ent.u.body) {
	    ent.u.body = copy(env, ent.u.body);
	    vec_at(env->symtab, i) = ent;
	}
	/* The last flag that used to be tested here, wasn't working. */
    }
}

static void gc1(pEnv env, Index *l, Index *r)
{
#ifdef TRACEGC
    if (env->tracegc > 1)
	printf("begin garbage collection\n");
    nodesinspected = nodescopied = 0;
#define COP1(X, NAME)						\
    if (X) {							\
	if (env->tracegc > 2) {					\
	    printf("\nold %s = ", NAME);			\
	    writeterm(env, X, stdout); putchar('\n'); } }
    COP1(env->stck,  "stack");
    COP1(env->prog,  "prog");
    COP1(env->conts, "conts");
    COP1(env->dump,  "dump");
    COP1(env->dump1, "dump1");
    COP1(env->dump2, "dump2");
    COP1(env->dump3, "dump3");
    COP1(env->dump4, "dump4");
    COP1(env->dump5, "dump5");
    if (l)
	COP1(*l,     "list");		/* copy parameters */
    if (r)
	COP1(*r,     "next");
#endif
    env->old_memory = env->memory;	/* backup original memory */
    /*
     * Allocate new memory with sufficient capacity.
     */
    env->memory = calloc(env->memorymax, sizeof(Node));
#ifdef TEST_MALLOC_RETURN
    if (!env->memory)
	fatal("memory exhausted");
#endif
    /* copy all nodes that are used in definitions */
    env->memoryindex = env->mem_low;
    memcpy(env->memory, env->old_memory, env->memoryindex * sizeof(Node));
#ifdef TRACEGC
#define COP2(X, NAME)						\
    if (X) { X = copy(env, X);					\
	if (env->tracegc > 2) {					\
	    printf("\nnew %s = ", NAME);			\
	    writeterm(env, X, stdout); putchar('\n'); } }
#else
#define COP2(X, NAME)						\
    if (X) X = copy(env, X)
#endif
    COP2(env->stck,  "stack");
    COP2(env->prog,  "prog");
    COP2(env->conts, "conts");
    COP2(env->dump,  "dump");
    COP2(env->dump1, "dump1");
    COP2(env->dump2, "dump2");
    COP2(env->dump3, "dump3");
    COP2(env->dump4, "dump4");
    COP2(env->dump5, "dump5");
    if (l)
	COP2(*l,     "list");	/* copy parameters */
    if (r)
	COP2(*r,     "next");
    if (env->variable_busy)	/* also copy variables, if there are any */
    	scan_roots(env);
}

static void gc2(pEnv env)
{
    clock_t this_gc_clock;

    free(env->old_memory);			/* release old memory */
    env->old_memory = 0;
    this_gc_clock = clock() - start_gc_clock;	/* statistics */
    env->gc_clock += this_gc_clock;
    env->collect++;
#ifdef TRACEGC
    if (env->tracegc > 0)
	printf("gc - %d nodes inspected, %d nodes copied, clock: %ld\n",
	    nodesinspected, nodescopied, this_gc_clock);
    if (env->tracegc > 1)
	printf("end garbage collection\n");
#endif
}

void my_gc(pEnv env)
{
    start_gc_clock = clock();	/* statistics */
    gc1(env, 0, 0);
    gc2(env);
}

/*
 * Count the number of physical nodes that need to be copied
 * during garbage collection, when the logical node at Index
 * n needs to be copied.
 */
static int count(pEnv env, Index n)
{
    Operator op;
    int size, num = 1;

/*
 * If n is 0 or in the space reserved for definitions, it is not counted.
 */
    if (n < env->mem_low)
	return 0;
    op = env->memory[n].op;
/*
 * If the node contains a string, then some more copying is needed.
 */
    if (op == STRING_ || op == BIGNUM_) {
	size = env->memory[n].len + 1;
	if ((size -= sizeof(Types)) > 0)	/* first part in Types */
	    num += (size + sizeof(Node) - 1) / sizeof(Node);	/* round up */
    }
/*
 * If the node contains a list, then the list needs to be counted as well.
 */
    if (op == LIST_)
	num += count(env, env->memory[n].u.lis);
/*
 * Also count the rest, just like this is done during copying.
 */
    num += count(env, env->memory[n].next);
/*
 * Return the number of physical nodes.
 */
    return num;
}

/*
 * Count the number of nodes that are used by variables. They are also copied.
 */
static int count_roots(pEnv env)
{
    Entry ent;
    int i, num = 0;

    /*
     * Look in the symbol table for variables. They also need to be garbage
     * collected.
     */
    for (i = vec_size(env->symtab) - 1; i > 0; i--) {
	ent = vec_at(env->symtab, i);
	if (!ent.is_user)	/* stop at builtins */
	    break;
	if (ent.is_root && ent.u.body)
	    num += count(env, ent.u.body);
    }
    return num;
}

/*
 * Allocate a number of nodes. The nodes are filled from the parameters.
 * Strings are passed in allocated memory, that is copied to nodes; they
 * must be freed afterwards.
 */
Index newnode(pEnv env, Operator o, Types u, Index r)
{
    Index p;
    size_t numgc;
    int size, leng = 0, num = 1;		/* allocate at least one node */

    if (o == STRING_ || o == BIGNUM_) {
	size = leng = strlen(u.str) + 1;
	if ((size -= sizeof(Types)) > 0)	/* first part in Types */
	    num += (size + sizeof(Node) - 1) / sizeof(Node);	/* round up */
    }
    if (env->memoryindex + num >= env->memorymax) {	/* space for new node */
	/*
	 * No garbage collection during the read of definitions.
	 */
	if (env->flibrary_busy) {
	    while (env->memoryindex + num >= env->memorymax)
		env->memorymax *= 2;
	    env->memory = realloc(env->memory, env->memorymax * sizeof(Node));
#ifdef TEST_MALLOC_RETURN
	    if (!env->memory)
		fatal("memory exhausted");
#endif
	} else {
	    /*
	     * Make sure that enough nodes are available.
	     * Simply count everything that will be copied.
	     * And because of all the counting, clock should
	     * be started here.
	     */
	    start_gc_clock = clock();		/* statistics */
	    numgc = env->mem_low + num;
    	    numgc += count(env, env->stck);
    	    numgc += count(env, env->prog);
    	    numgc += count(env, env->conts);
    	    numgc += count(env, env->dump);
    	    numgc += count(env, env->dump1);
    	    numgc += count(env, env->dump2);
    	    numgc += count(env, env->dump3);
    	    numgc += count(env, env->dump4);
    	    numgc += count(env, env->dump5);
	    if (o == LIST_)
		numgc += count(env, u.lis);
	    numgc += count(env, r);
	    numgc += count_roots(env);
	    /*
	     * Check whether memory can be decreased or whether it must be
	     * increased or whether it can stay the same. The aim is not to
	     * claim more memory than necessary.
	     */
	    if (numgc * 100.0 / env->memorymax < 10)	/* check decrease */
		env->memorymax *= 0.9;
	    else if (numgc * 100.0 / env->memorymax > 90) {	/* increase */
		if (env->memorymax < INT_MAX)
		    env->memorymax *= 2;
		else
		    env->memorymax = INT_MAX;
	    }
	    /*
	     * Make sure that enough nodes are available. Just in case the
	     * decrease calculation was wrong.
	     */
	    if (env->memorymax < numgc)
		env->memorymax = numgc;
	    /*
	     * Also make sure that memory is not lower than MEM_LOW.
	     */
	    if (env->memorymax < MEM_LOW)
		env->memorymax = MEM_LOW;
	    /*
	     * Now do the actual copying. The new memory, that is guaranteed to
	     * be large enough, is allocated in gc1. In gc2 the old memory will
	     * be released.
	     */
	    if (o == LIST_)		/* copy parameters */
		gc1(env, &u.lis, &r);
	    else			/* copy roots */
		gc1(env, 0, &r);
	    /*
	     * Check that, after copying, memoryindex is lower than memorymax.
	     * It should be, because the precise number was counted up front.
	     * And if it wasn't then some memory may have been overwritten.
	     */
	    assert(env->memoryindex < env->memorymax);
	    gc2(env);		/* finish copying */
	}
    }
    p = env->memoryindex;	/* new index of new node(s) */
    env->memoryindex += num;	/* space for new node(s) */
    env->nodes += num;		/* statistics */
/*
 * Fill newly created node(s) with data from parameters. Some special handling
 * is needed in case of strings.
 */
    env->memory[p].u = u;
    env->memory[p].op = o;
    env->memory[p].next = r;
    if (o == STRING_ || o == BIGNUM_) {
	memcpy(&env->memory[p].u, u.str, leng);
	env->memory[p].len = leng - 1;
    }
    return p;
}

/*
 * In the G-macro's, nodes are taken from the stack, where strings are not
 * allocated, but interned. It is sufficient to redirect these circumstances
 * to the normal newnode. The index of the nodes must be passed, not separated
 * in type and value.
 */
Index newnode2(pEnv env, Index p, Index r)
{
    Types u;
    Operator o;

    if ((o = env->memory[p].op) == STRING_ || o == BIGNUM_)
	u.str = check_strdup((char *)&env->memory[p].u);
    else
	u = env->memory[p].u;
    p = newnode(env, o, u, r);
    if (o == STRING_ || o == BIGNUM_)
	free(u.str);
    return p;
}

/*
 * Report the number of nodes that are currently being used.
 */
void my_memoryindex(pEnv env)
{
    NULLARY(INTEGER_NEWNODE, env->memoryindex);
}

/*
 * Report the total number of nodes that are currently available.
 */
void my_memorymax(pEnv env)
{
    NULLARY(INTEGER_NEWNODE, env->memorymax);
}
