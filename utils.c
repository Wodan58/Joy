/*
 *  module  : utils.c
 *  version : 1.47
 *  date    : 01/14/25
 */
#include "globals.h"

#if 0
#define TRACEGC
#endif

/*
 * MEM_LOW should allow reading usrlib.joy, inilib.joy, prelib.joy, and
 * agglib.joy without reallocation and without garbage collection.
 */
#define MEM_LOW		1100	/* initial number of nodes */

static Node *old_memory;
static size_t memorymax;
static Index mem_low, memoryindex;

static clock_t start_gc_clock;	/* statistics */
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
#ifdef TRACEGC
    env->tracegc = 6;		/* set to maximum */
#endif    
    if (!mem_low) {
	memoryindex = mem_low = 1;
	env->memory = calloc(memorymax = MEM_LOW, sizeof(Node));
#ifdef _MSC_VER
	if (!env->memory)
	    fatal("memory exhausted");
#endif
    } else if (status) {
	env->stck = env->inits;	/* reset the stack to initial */
	memoryindex = mem_low;	/* retain only definitions */
    }
    env->conts = env->dump = 0;
    env->dump1 = env->dump2 = env->dump3 = env->dump4 = env->dump5 = 0;
    env->flibrary_busy = 1;	/* disable garbage collection */
}

/*
 * Reset mem_low after reading a definition. This invalidates all allocations
 * higher than mem_low: program, stack, dumps.
 */
void inimem2(pEnv env)
{
    double new_avail;

    mem_low = memoryindex;	/* enlarge definition space */
    new_avail = memorymax - mem_low;
    if (env->avail > new_avail || !env->avail)
	env->avail = new_avail;
    env->flibrary_busy = 0;	/* enable garbage collection */
#ifdef TRACEGC
    if (env->tracegc > 1) {
	printf("mem_low = %d\n", mem_low);
	printf("memoryindex = %d\n", memoryindex);
	printf("top of mem = %d\n", memorymax);
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
    Index temp;
    Operator op;
    int size, leng, num = 1;

#ifdef TRACEGC
    nodesinspected++;
    if (env->tracegc > 4)
	printf("copy .. (%d)\n", n);
#endif
/*
 * If n is 0 or in the space reserved for definitions, it is returned
 * unmodified. Definitions have already been copied.
 */
    if (n < mem_low)
	return n;
/*
 * If the node has already been copied, then the new index is present in the
 * lis member.
 */
    if ((op = old_memory[n].op) == COPIED_)
	return old_memory[n].u.lis;
/*
 * Copy the node from the original to the new location. What gets copied is:
 * op, len, next, u.
 */
    env->memory[temp = memoryindex++] = old_memory[n];
/*
 * If the node contains a string, then some more copying is needed.
 */
    if (op == STRING_ || op == BIGNUM_) {
	size = leng = old_memory[n].len + 1;
	memcpy(&env->memory[temp].u, &old_memory[n].u, leng);
	if ((size -= sizeof(Types)) > 0)	/* first part in Types */
	    num += (size + sizeof(Node) - 1) / sizeof(Node);	/* round up */
	memoryindex += num;
    }
/*
 * If the node contains a list, then the list needs to be copied. This requires
 * that copy is called recursively.
 */
    if (op == LIST_)
	env->memory[temp].u.lis = copy(env, old_memory[n].u.lis);
/*
 * The next field also needs to be copied separately, overruling what was there
 * copied earlier on.
 */
    env->memory[temp].next = copy(env, old_memory[n].next);
/*
 * The original location is set to COPIED_, such that it will not be copied
 * again.
 */
    old_memory[n].op = COPIED_;
    old_memory[n].u.lis = temp;
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
	if (ent.is_last)		/* stop at the last variable */
	    break;
    }
}

static void gc1(pEnv env)
{
    start_gc_clock = clock();		/* statistics */
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
#endif
    old_memory = env->memory;		/* backup original memory */
    /* allocate new memory with at least the same capacity as the original */
    env->memory = calloc(memorymax, sizeof(Node));
#ifdef _MSC_VER
    if (!env->memory)
	fatal("memory exhausted");
#endif
    /* copy all nodes that are used in definitions */
    memcpy(env->memory, old_memory, (memoryindex = mem_low) * sizeof(Node));
#ifdef TRACEGC
#define COP2(X, NAME)						\
    if (X) {							\
	X = copy(env, X);					\
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

    if (env->variable_busy)	/* also copy variables, if there are any */
	scan_roots(env);
}

static void gc2(pEnv env)
{
    clock_t this_gc_clock;

    free(old_memory);		/* release old memory */
/*
 * If memory is mostly occupied, it should be increased. If only a small amount
 * is occupied, it should be decreased.
 */
    if (memoryindex * 100.0 / memorymax < 10)	/* check decrease */
	memorymax *= 0.9;
    else if (memoryindex * 100.0 / memorymax > 90) {	/* check increase */
	env->memory = realloc(env->memory, (memorymax *= 2) * sizeof(Node));
#ifdef _MSC_VER
	if (!env->memory)
	    fatal("memory exhausted");
#endif
    }
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
    gc1(env);
    gc2(env);
}

/*
 * Allocate a number of nodes. The nodes are filled from the parameters.
 * Strings are passed in allocated memory, that is copied to nodes and must
 * be freed afterwards.
 */
Index newnode(pEnv env, Operator o, Types u, Index r)
{
    Index p;
    int size, leng = 0, num = 1;	/* allocate at least one node */

    if (o == STRING_ || o == BIGNUM_) {
	size = leng = strlen(u.str) + 1;
	if ((size -= sizeof(Types)) > 0)	/* first part in Types */
	    num += (size + sizeof(Node) - 1) / sizeof(Node);	/* round up */
    }
    if (memoryindex + num >= memorymax) {	/* space for new node */
	if (env->flibrary_busy) {
	    while (memoryindex + num >= memorymax)
		memorymax *= 2;
	    env->memory = realloc(env->memory, memorymax * sizeof(Node));
#ifdef _MSC_VER
	    if (!env->memory)
		fatal("memory exhausted");
#endif
	} else {
	    gc1(env);		/* copy roots */
	    if (o == LIST_)	/* copy parameters */
		u.lis = copy(env, u.lis);
	    r = copy(env, r);
	    gc2(env);		/* finish copying */
	}
    }
    p = memoryindex;		/* new index of new node(s) */
    memoryindex += num;		/* space for new node(s) */
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

    if ((o = env->memory[p].op) == STRING_ || o == BIGNUM_) {
	u.str = strdup((char *)&env->memory[p].u);
#ifdef _MSC_VER
	if (!u.str)
	    fatal("memory exhausted");
#endif
    } else
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
    NULLARY(INTEGER_NEWNODE, memoryindex);
}

/*
 * Report the total number of nodes that are currently available.
 */
void my_memorymax(pEnv env)
{
    NULLARY(INTEGER_NEWNODE, memorymax);
}
