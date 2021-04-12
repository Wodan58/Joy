/*
    module  : gc.c
    version : 1.20
    date    : 03/12/21
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <setjmp.h>
#include <math.h>
#include "gc.h"
#include "khash.h"

#define GC_LEAF		1
#define GC_MARK		2

#define MIN_ITEMS	4

typedef struct mem_info {
    unsigned char flags;
    size_t size;
} mem_info;

/*
    When pointers are 16 bit aligned, the lower 4 bits are always zero.
*/
#define HASH_FUNCTION(key)	((key) >> 4)

/*
    The map contains a pointer as key and mem_info as value.
*/
KHASH_INIT(Backup, uintptr_t, mem_info, 1, HASH_FUNCTION, kh_int64_hash_equal)

static khash_t(Backup) *MEM;			    /* backup of pointers */
static char *bottom;				    /* stack bottom */
static khint_t max_items = MIN_ITEMS;		    /* max. items before gc */
static char *lower = (char *)UINTPTR_MAX, *upper;   /* lower and upper heap */

/*
    Report a fatal error and abort execution.
*/
static void mem_fatal(void)
{
    fprintf(stderr, "Out of memory\n");
    abort();
}

/*
    Report of the amount of memory allocated is delegated to valgrind.
*/
static void mem_exit(void)
{
    khiter_t key;

    for (key = kh_begin(MEM); key != kh_end(MEM); key++)
	if (kh_exist(MEM, key))
	    free((void *)kh_key(MEM, key));
    kh_destroy(Backup, MEM);
}

/*
    Initialise gc memory.
*/
void GC_init(void *ptr)
{
    bottom = ptr;
    atexit(mem_exit);
    MEM = kh_init(Backup);
}

/*
    Walk registered blocks and free those that have not been marked.
*/
static void scan(void)
{
    khiter_t key;

    for (key = kh_begin(MEM); key != kh_end(MEM); key++)
	if (kh_exist(MEM, key)) {
	    if (kh_value(MEM, key).flags & GC_MARK)
		kh_value(MEM, key).flags &= ~GC_MARK;
	    else {
		free((void *)kh_key(MEM, key));
		kh_del(Backup, MEM, key);
	    }
	}
    max_items = 2 * kh_size(MEM);
    if (max_items < MIN_ITEMS)
	max_items = MIN_ITEMS;
}

/*
    Mark a block as in use.
*/
static void mark_ptr(char *ptr)
{
    khiter_t key;
    size_t i, size;

    if (ptr < lower || ptr > upper)
	return;
    if ((key = kh_get(Backup, MEM, (uintptr_t)ptr)) != kh_end(MEM)) {
	if (kh_value(MEM, key).flags & GC_MARK)
	    return;
	kh_value(MEM, key).flags |= GC_MARK;
	if (kh_value(MEM, key).flags & GC_LEAF)
	    return;
	size = kh_value(MEM, key).size / sizeof(char *);
	for (i = 0; i < size; i++)
	    mark_ptr(((char **)ptr)[i]);
    }
}

/*
    Mark blocks that can be found on the stack.
*/
static void mark_stk(void)
{
    char *ptr = (char *)&ptr;
  
#ifdef STACK_GROWS_UPWARD
    if (ptr > bottom)
	for (; ptr > bottom; ptr -= sizeof(char *))
	    mark_ptr(*(char **)ptr);
    else
#endif
	for (; ptr < bottom; ptr += sizeof(char *))
	    mark_ptr(*(char **)ptr);
}

/*
    Collect garbage.

    Pointers that are reachable from registers or stack are marked
    as well as all pointers that are reachable from those pointers.
    In other words: roots for garbage collection are searched in
    registers and on the stack.
*/
void GC_gcollect(void)
{
    jmp_buf env;
    void (* volatile m)(void) = mark_stk;

    memset(&env, 0, sizeof(jmp_buf));
    setjmp(env);
    (*m)();
    scan();
}

/*
    Register an allocated memory block.
*/
static void remind(char *ptr, size_t size, int leaf)
{
    int rv;
    khiter_t key;

    if (lower > ptr)
	lower = ptr;
    if (upper < ptr + size)
	upper = ptr + size;
    if (kh_size(MEM) > max_items)
	GC_gcollect();
    key = kh_put(Backup, MEM, (uintptr_t)ptr, &rv);
    kh_value(MEM, key).flags = leaf;
    kh_value(MEM, key).size = size;
}

/*
    Register an allocated block.
*/
static void *mem_block(size_t size, int f)
{
    void *ptr;

    if ((ptr = malloc(size)) == 0)
	mem_fatal();
    memset(ptr, 0, size);
    remind(ptr, size, f);
    return ptr;
}

/*
    Register an atomic allocated block.
*/
#ifdef USE_GC_MALLOC_ATOMIC
void *GC_malloc_atomic(size_t size)
{
    return mem_block(size, 1);
}
#endif

/*
    Register a memory block.
*/
#ifdef USE_GC_MALLOC
void *GC_malloc(size_t size)
{
    return mem_block(size, 0);
}
#endif

/*
    Update the size of a memory block.
*/
#ifdef USE_GC_REALLOC
static void update(void *ptr, size_t size)
{
    khiter_t key;

    if ((key = kh_get(Backup, MEM, (uintptr_t)ptr)) != kh_end(MEM))
	kh_value(MEM, key).size = size;
}

/*
    Forget about a memory block.
*/
static unsigned char forget(void *ptr)
{
    khiter_t key;
    unsigned char flags = 0;

    if ((key = kh_get(Backup, MEM, (uintptr_t)ptr)) != kh_end(MEM)) {
	flags = kh_value(MEM, key).flags;
	kh_del(Backup, MEM, key);
    }
    return flags;
}

/*
    Enlarge an already allocated memory block.
*/
void *GC_realloc(void *old, size_t size)
{
    void *ptr;

    if ((ptr = realloc(old, size)) == 0)
	mem_fatal();
    if (ptr == old)
	update(ptr, size);
    else
	remind(ptr, size, forget(old));
    return ptr;
}
#endif

/*
    Duplicate a string.
*/
#ifdef USE_GC_STRDUP
char *GC_strdup(const char *str)
{
    char *ptr;
    size_t leng;

    leng = strlen(str);
    if ((ptr = GC_malloc_atomic(leng + 1)) != 0)
	strcpy(ptr, str);
    return ptr;
}
#endif
