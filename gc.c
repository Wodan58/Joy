/*
    module  : gc.c
    version : 1.35
    date    : 08/06/23
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach-o/getsect.h>
#endif

#include "khash.h"
#include "gc.h"

#ifdef _MSC_VER
#define DOWN_64K        ~0xFFFF
#define PEPOINTER       15
#define IMAGE_BASE      13
#define BASE_OF_CODE    11
#define SIZE_OF_CODE     7
#define SIZE_OF_DATA     8
#define SIZE_OF_BSS      9
#endif

#define GC_COLL          0
#define GC_LEAF          1
#define GC_MARK          2

#define BSS_ALIGN        4
#define MIN_ITEMS        4
#define MAX_ITEMS        2
#define FULL_MASK        (uint64_t)0x0000fffffffffffe

/*
    When pointers are 16 bit aligned, the lower 4 bits are always zero.
*/
#define HASH_FUNCTION(key)        (khint_t)((key) >> 4)

typedef struct mem_info {
    unsigned flags: 2;
    unsigned size: 30;
} mem_info;

/*
    The map contains a pointer as key and mem_info as value.
*/
KHASH_INIT(Backup, uint64_t, mem_info, 1, HASH_FUNCTION, kh_int64_hash_equal)

static khint_t max_items;                /* max. items before gc      */
static khash_t(Backup) *MEM;             /* backup of pointers        */
static uint64_t bottom, lower, upper;    /* stack bottom, heap bounds */

/*
    Pointers to memory segments.
*/
#ifdef SCAN_BSS_MEMORY
static uint64_t start_of_text,
                start_of_data,
                start_of_bss,
                start_of_heap;
#endif

/*
    mem_fatal - Report a fatal error and end the program. The message is taken
                from yacc.
*/
static void mem_fatal(void)
{
    fprintf(stderr, "memory exhausted\n");
    exit(0);
}

/*
    Determine sections of memory.
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
    Report of the amount of memory allocated is delegated to valgrind.
*/
#ifdef FREE_ON_EXIT
static void mem_exit(void)
{
    khiter_t key;

    for (key = kh_begin(MEM); key != kh_end(MEM); key++)
        if (kh_exist(MEM, key))
            free((void *)kh_key(MEM, key));
    kh_destroy(Backup, MEM);
}
#endif

/*
    Initialise gc memory.
*/
void GC_init(void *ptr)
{
#ifdef SCAN_BSS_MEMORY
    init_heap();
#endif
#ifdef FREE_ON_EXIT
    atexit(mem_exit);
#endif
    bottom = (uint64_t)ptr;
    MEM = kh_init(Backup);
    max_items = MIN_ITEMS;
}

/*
    Mark a block as in use. No optimization for this (recursive) function.
*/
static void mark_ptr(char *ptr)
{
    khiter_t key;
    size_t i, size;
    uint64_t value;

    value = (uint64_t)ptr;
    value &= FULL_MASK;
    if (value < lower || value >= upper)
        return;
    if ((key = kh_get(Backup, MEM, value)) != kh_end(MEM)) {
        if (kh_value(MEM, key).flags & GC_MARK)
            return;
        kh_value(MEM, key).flags |= GC_MARK;
        if (kh_value(MEM, key).flags & GC_LEAF)
            return;
        size = kh_value(MEM, key).size / sizeof(char *);
        for (i = 0; i < size; i++)
            mark_ptr(((char **)value)[i]);
    }
}

/*
    Mark blocks that can be found on the stack.
*/
static void mark_stk(void)
{
    uint64_t ptr = (uint64_t)&ptr;
  
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
    Mark blocks that are pointed to from static uninitialized memory.
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
    Walk registered blocks and free those that have not been marked, unless
    they are marked as uncollectable.
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
}

/*
    Collect garbage.

    Pointers that are reachable from registers or stack are marked
    as well as all pointers that are reachable from those pointers.
    In other words: roots for garbage collection are searched in
    registers, on the stack, and in the blocks themselves.
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
}

/*
    Register an allocated memory block and garbage collect if there are too
    many blocks already.
*/
static void remind(char *ptr, size_t size, int flags)
{
    int rv;
    khiter_t key;
    uint64_t value;

    value = (uint64_t)ptr;
    if (lower > value || !lower)
        lower = value;
    if (upper < value + size)
        upper = value + size;
    key = kh_put(Backup, MEM, value, &rv);
    kh_value(MEM, key).flags = flags;
    kh_value(MEM, key).size = size;
    if (max_items < kh_size(MEM)) {
        GC_gcollect();
        max_items = kh_size(MEM) * MAX_ITEMS;
    }
}

/*
    Register an allocated memory block. The block is cleared with zeroes.
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
    Register a memory block that contains no other blocks.
*/
#ifdef USE_GC_MALLOC_ATOMIC
void *GC_malloc_atomic(size_t size)
{
    return mem_block(size, GC_LEAF);
}
#endif

/*
    Register a memory block that can be collected.
*/
#ifdef USE_GC_MALLOC
void *GC_malloc(size_t size)
{
    return mem_block(size, GC_COLL);
}
#endif

/*
    Update the size of a memory block.
*/
#ifdef USE_GC_REALLOC
static void update(void *ptr, size_t size)
{
    khiter_t key;

    if ((key = kh_get(Backup, MEM, (uint64_t)ptr)) != kh_end(MEM))
        kh_value(MEM, key).size = size;
}

/*
    Forget about a memory block and return its flags.
*/
static unsigned char forget(void *ptr)
{
    khiter_t key;
    unsigned char flags = 0;

    if ((key = kh_get(Backup, MEM, (uint64_t)ptr)) != kh_end(MEM)) {
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

    if (!old)
        return GC_malloc(size);
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
    Duplicate a string. A string does not contain internal pointers.
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

#ifdef USE_GC_GET_HEAP_SIZE
/*
    Return the number of memory blocks.
*/
size_t GC_get_heap_size(void)
{
    return max_items;
}

/*
    Return the number of bytes allocated.
*/
size_t GC_get_memory_use(void)
{
    return upper - lower;
}
#endif
