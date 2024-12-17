/*
    module  : gc.h
    version : 1.24
    date    : 12/15/24
*/
#ifndef GC_H
#define GC_H

/* These definitions should be present in the main function */
extern char *bottom_of_stack;
void fatal(char *str);

/* #define SCAN_BSS_MEMORY */
/* #define STACK_GROWS_UPWARD */
/* #define USE_GC_REALLOC */

#ifdef MALLOC_DEBUG
#define FREE_ON_EXIT
#endif
#define USE_GC_MALLOC_ATOMIC
#define USE_GC_MALLOC
#define USE_GC_STRDUP
#define COUNT_COLLECTIONS

/* GC_INIT must be called in the main function */
void GC_INIT(void);
void GC_gcollect(void);
void *GC_malloc_atomic(size_t size);
void *GC_malloc(size_t size);
void *GC_realloc(void *old, size_t size);
char *GC_strdup(const char *str);
size_t GC_get_gc_no(void);
size_t GC_get_memory_use(void);
size_t GC_get_free_bytes(void);
#endif
