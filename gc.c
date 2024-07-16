/*
    module  : gc.c
    version : 1.51
    date    : 07/01/24
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>

/*
 * khashl.h is not officially supported.
 */
#if 0
#define USE_KHASHL
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4267)
#define kh_packed		/* forget about __attribute__ ((packed)) */
#endif

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach-o/getsect.h>
#endif

#ifdef USE_KHASHL
#include "khashl.h"
#else
#include "khash.h"
#endif
#include "gc.h"

#ifdef _MSC_VER
#define DOWN_64K	~0xFFFF
#define PEPOINTER	15
#define IMAGE_BASE	13
#define BASE_OF_CODE	11
#define SIZE_OF_CODE	7
#define SIZE_OF_DATA	8
#define SIZE_OF_BSS	9
#endif

#define GC_COLL		0
#define GC_LEAF		1
#define GC_MARK		2

#define GROW_FACTOR	2
#define BSS_ALIGN	4
#define MIN_ITEMS	170	/* initial number of items */

/*
 * When pointers are aligned at 16 bytes, the lower 4 bits are always zero.
 */
#define HASH_FUNCTION(key)	(khint_t)((key) >> 4)

typedef struct mem_info {
    unsigned flags: 2;
    unsigned size: 30;
} mem_info;

/*
 * The map contains a pointer as key and mem_info as value.
 */
#ifdef USE_KHASHL
KHASHL_MAP_INIT(KH_LOCAL, backup_t, backup, uint64_t, mem_info, HASH_FUNCTION,
		kh_eq_generic)

static backup_t *MEM;		/* backup of pointers */
#else
KHASH_INIT(Backup, uint64_t, mem_info, 1, HASH_FUNCTION, kh_int64_hash_equal)

static khash_t(Backup) *MEM;	/* backup of pointers */
#endif

static khint_t max_items;	/* max. items before gc */
static uint64_t lower, upper;	/* heap bounds */
#ifdef COUNT_COLLECTIONS
static size_t GC_gc_no;		/* number of garbage collections */
#endif

/*
 * Pointers to memory segments.
 */
#ifdef SCAN_BSS_MEMORY
static uint64_t start_of_text,
		start_of_data,
		start_of_bss,
		start_of_heap;
#endif

/*
 * fatal - Report a fatal error and end the program. The message is taken from
 *	   yacc. The function should be present in main.c
 */
#ifdef _MSC_VER
void fatal(char *str);
#endif

/*
 * Determine sections of memory. This is highly system dependent and not tested
 * on __APPLE__.
 */
#ifdef SCAN_BSS_MEMORY
static void init_heap(void)
{
    extern int main(int argc, char **argv);
#ifdef _MSC_VER
    int *ptr;
#endif
    start_of_text = (uint64_t)main;
#ifdef __CYGWIN__
    extern char __data_start__, __bss_start__, __bss_end__;
#if 0
    extern char __data_end__, __end__;
#endif
    start_of_data = (uint64_t)&__data_start__;
    start_of_bss  = (uint64_t)&__bss_start__;
    start_of_heap = (uint64_t)&__bss_end__;
#endif
#ifdef __linux__
    extern char etext, edata, end;

    start_of_data = (uint64_t)&etext;
    start_of_bss  = (uint64_t)&edata;
    start_of_heap = (uint64_t)&end;
#endif
#ifdef __APPLE__
    start_of_data = (uint64_t)get_etext();
    start_of_bss  = (uint64_t)get_edata();
    start_of_heap = (uint64_t)get_end();
#endif
#ifdef _MSC_VER
    start_of_text &= DOWN_64K;
    ptr = (int *)start_of_text;
    ptr += ptr[PEPOINTER] / 4;
    start_of_text = ptr[IMAGE_BASE] + ptr[BASE_OF_CODE];
    start_of_data = start_of_text + ptr[SIZE_OF_CODE];
    start_of_bss  = start_of_data + ptr[SIZE_OF_DATA];
    start_of_heap = start_of_bss + ptr[SIZE_OF_BSS];
#endif
}
#endif

/*
 * Report of the amount of memory allocated is delegated to valgrind.
 */
#ifdef FREE_ON_EXIT
static void mem_exit(void)
{
    khint_t key;

    for (key = 0; key != kh_end(MEM); key++)
	if (kh_exist(MEM, key))
	    free((void *)kh_key(MEM, key));
#ifdef USE_KHASHL
    backup_destroy(MEM);
#else
    kh_destroy(Backup, MEM);
#endif
}
#endif

/*
 * Initialise gc memory.
 */
void GC_INIT(void)
{
#ifdef SCAN_BSS_MEMORY
    init_heap();
#endif
#ifdef FREE_ON_EXIT
    atexit(mem_exit);
#endif
#ifdef USE_KHASHL
    MEM = backup_init();
#else
    MEM = kh_init(Backup);
#endif
    max_items = MIN_ITEMS;
}

/*
 * Mark a block as in use. No optimization for this (recursive) function.
 */
static void mark_ptr(char *ptr)
{
    khint_t key;
    size_t i, size;
    uint64_t value;

    value = (uint64_t)ptr;
    if (value < lower || value >= upper)
	return;
#ifdef USE_KHASHL
    if ((key = backup_get(MEM, value)) != kh_end(MEM)) {
#else
    if ((key = kh_get(Backup, MEM, value)) != kh_end(MEM)) {
#endif
	if (kh_val(MEM, key).flags & GC_MARK)
	    return;
	kh_val(MEM, key).flags |= GC_MARK;
	if (kh_val(MEM, key).flags & GC_LEAF)
	    return;
	size = kh_val(MEM, key).size / sizeof(char *);
	for (i = 0; i < size; i++)
	    mark_ptr(((char **)value)[i]);	/* recursion is suspicious */
    }
}

/*
 * Mark blocks that can be found on the stack.
 */
static void mark_stk(void)
{
    uint64_t ptr = (uint64_t)&ptr;
  
#ifdef STACK_GROWS_UPWARD
    if (ptr > bottom)
	for (; ptr > (uint64_t)bottom_of_stack; ptr -= sizeof(char *))
	    mark_ptr(*(char **)ptr);
    else
#endif
	for (; ptr < (uint64_t)bottom_of_stack; ptr += sizeof(char *))
	    mark_ptr(*(char **)ptr);
}

/*
 * Mark blocks that are pointed to from static uninitialized memory.
 */
#ifdef SCAN_BSS_MEMORY
static void mark_bss(void)
{
    uint64_t ptr, end_of_bss;

    end_of_bss = start_of_heap - sizeof(void *);
    for (ptr = start_of_bss; ptr <= end_of_bss; ptr += BSS_ALIGN)
	mark_ptr(*(char **)ptr);
}
#endif

/*
 * Walk registered blocks and free those that have not been marked.
 */
static void scan(void)
{
    khint_t key;

    for (key = 0; key != kh_end(MEM); key++) {
	if (kh_exist(MEM, key)) {
	    if (kh_val(MEM, key).flags & GC_MARK)
		kh_val(MEM, key).flags &= ~GC_MARK;
	    else {
		free((void *)kh_key(MEM, key));
#ifdef USE_KHASHL
		backup_del(MEM, key--);	/* delete in kh_foreach is suspicious */
#else
		kh_del(Backup, MEM, key);
#endif
	    }
	}
    }
}

/*
 * Collect garbage.
 *
 * Pointers that are reachable from registers or stack are marked
 * as well as all pointers that are reachable from those pointers.
 * In other words: roots for garbage collection are searched in
 * registers, on the stack, and in the blocks themselves.
 */
void GC_gcollect(void)
{
    jmp_buf env;
    void (* volatile m)(void) = mark_stk;

    memset(&env, 0, sizeof(jmp_buf));
    setjmp(env);
    (*m)();
#ifdef SCAN_BSS_MEMORY
    mark_bss();
#endif
    scan();
#ifdef COUNT_COLLECTIONS
    GC_gc_no++;
#endif
}

/*
 * Register an allocated memory block and garbage collect if there are too many
 * blocks already.
 */
static void remind(char *ptr, size_t size, int flags)
{
    int rv;
    khint_t key;
    uint64_t value;

    value = (uint64_t)ptr;
    if (lower > value || !lower)
	lower = value;
    if (upper < value + size)
	upper = value + size;
#ifdef USE_KHASHL
    key = backup_put(MEM, value, &rv);
#else
    key = kh_put(Backup, MEM, value, &rv);
#endif
    kh_val(MEM, key).flags = flags;
    kh_val(MEM, key).size = size;
/*
 * See if there are already too many items allocated. If yes, trigger the
 * garbage collector. As the number of items that need to be remembered is
 * unknown, it is set to twice the number of items that are currently being
 * used. This allows a 100% growth in the number of items allocated.
 */
    if (max_items < kh_size(MEM)) {
	GC_gcollect();
	max_items = kh_size(MEM) * GROW_FACTOR;
    }
}

/*
 * Register an allocated memory block. The block is cleared with zeroes.
 */
static void *mem_block(size_t size, int leaf)
{
    void *ptr = 0;

    ptr = malloc(size);
#ifdef _MSC_VER
    if (!ptr)
	fatal("memory exhausted");
#endif
    memset(ptr, 0, size);
    remind(ptr, size, leaf);
    return ptr;
}

/*
 * Register a memory block that contains no other blocks.
 */
#ifdef USE_GC_MALLOC_ATOMIC
void *GC_malloc_atomic(size_t size)
{
    return mem_block(size, GC_LEAF);
}
#endif

/*
 * Register a memory block that can be collected.
 */
#ifdef USE_GC_MALLOC
void *GC_malloc(size_t size)
{
    return mem_block(size, GC_COLL);
}
#endif

#ifdef USE_GC_REALLOC
/*
 * Forget about a memory block and return its flags.
 */
static unsigned char forget(void *ptr)
{
    khint_t key;
    unsigned char flags = 0;

#ifdef USE_KHASHL
    if ((key = backup_get(MEM, (uint64_t)ptr)) != kh_end(MEM)) {
#else
    if ((key = kh_get(Backup, MEM, (uint64_t)ptr)) != kh_end(MEM)) {
#endif
	flags = kh_val(MEM, key).flags;
#ifdef USE_KHASHL
	backup_del(MEM, key);
#else
	kh_del(Backup, MEM, key);
#endif
    }
    return flags;
}

/*
 * Enlarge an already allocated memory block.
 */
void *GC_realloc(void *ptr, size_t size)
{
    unsigned char flags;

    if (!ptr)
	return GC_malloc(size);
    flags = forget(ptr);
    ptr = realloc(ptr, size);
#ifdef _MSC_VER
    if (!ptr)
	fatal("memory exhausted");
#endif
    remind(ptr, size, flags);
    return ptr;
}
#endif

/*
 * Duplicate a string. A string does not contain internal pointers.
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

#ifdef COUNT_COLLECTIONS
/*
 * Return the number of garbage collections.
 */
size_t GC_get_gc_no(void)
{
    return GC_gc_no;
}
#endif
