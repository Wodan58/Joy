/*
    module  : gc.h
    version : 1.10
    date    : 04/09/22
*/
#ifndef GC_H
#define GC_H

#define FREE_ON_EXIT
/* #define SCAN_BSS_MEMORY */
/* #define STACK_GROWS_UPWARD	*/
/* #define USE_GC_MALLOC_UNCOLLECTABLE */
#define USE_GC_MALLOC_ATOMIC
#define USE_GC_MALLOC
#define USE_GC_REALLOC
#define USE_GC_STRDUP

void GC_init(void *ptr);
void GC_gcollect(void);
void *GC_malloc_uncollectable(size_t size);
void *GC_malloc_atomic(size_t size);
void *GC_malloc(size_t size);
void *GC_realloc(void *old, size_t size);
void *GC_realloc_uncollectable(void *old, size_t size);
char *GC_strdup(const char *str);
#endif
