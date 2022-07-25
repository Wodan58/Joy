/*
    module  : gc.h
    version : 1.15
    date    : 07/25/22
*/
#ifndef GC_H
#define GC_H

#define MAX_BLOCK	100000000

#define USE_GC_GET_HEAP_SIZE
#if defined(__linux__) || defined(__CYGWIN__)
#define SIGNAL_HANDLING
#endif
/* #define FREE_ON_EXIT */
/* #define SCAN_BSS_MEMORY */
/* #define STACK_GROWS_UPWARD */
/* #define USE_GC_MALLOC_UNCOLLECTABLE */
#define USE_GC_MALLOC_ATOMIC
#define USE_GC_MALLOC
#define USE_GC_REALLOC
#define USE_GC_STRDUP

void GC_init(void *ptr, void (*proc)());
void GC_gcollect(void);
void *GC_malloc_atomic(size_t size);
void *GC_malloc(size_t size);
void *GC_realloc(void *old, size_t size);
char *GC_strdup(const char *str);
size_t GC_get_heap_size(void);
size_t GC_get_memory_use(void);
void *GC_malloc_uncollectable(size_t size);
#endif
