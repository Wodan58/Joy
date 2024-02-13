/*
    module  : gc.h
    version : 1.20
    date    : 02/12/24
*/
#ifndef GC_H
#define GC_H

/* this should be provided by the main function */
extern char *bottom_of_stack;

/* #define SCAN_BSS_MEMORY */
/* #define FREE_ON_EXIT */
/* #define STACK_GROWS_UPWARD */
/* #define USE_GC_GET_HEAP_SIZE */
#define COUNT_COLLECTIONS
#define USE_GC_MALLOC_ATOMIC
#define USE_GC_MALLOC
#define USE_GC_REALLOC
#define USE_GC_STRDUP

void GC_INIT(void);
void GC_gcollect(void);
void *GC_malloc_atomic(size_t size);
void *GC_malloc(size_t size);
void *GC_realloc(void *old, size_t size);
char *GC_strdup(const char *str);
size_t GC_get_free_bytes(void);
size_t GC_get_memory_use(void);
size_t GC_get_gc_no(void);
#endif
