/*
    module  : gc.h
    version : 1.27
    date    : 01/07/26
*/
#ifndef GC_H
#define GC_H

/*
 * Under Linux overcommit_memory can be turned off (value 2), or ulimit kan be
 * set and that way, malloc can return 0. The return value of GC_malloc is not
 * tested, because, although BDW can announce that it will return NULL, it
 * never does.
 */
#define TEST_MALLOC_RETURN

/* These definitions should be present in the source file where main resides */
extern char *bottom_of_stack;
#ifdef NPROTO
void fatal();
#else
void fatal(char *str);			/* LCOV_EXCL_LINE */
#endif

/* #define STACK_GROWS_UPWARD */
/* #define SCAN_BSS_MEMORY */
/* #define USE_GC_REALLOC */

#ifdef MALLOC_DEBUG
#define FREE_ON_EXIT
#endif
#define USE_GC_MALLOC_ATOMIC
#define USE_GC_MALLOC
#define USE_GC_STRDUP
#define COUNT_COLLECTIONS

/* GC_INIT must be called in the main function */
#ifdef NPROTO
void GC_INIT();
void GC_gcollect();
void *GC_malloc_atomic();
void *GC_malloc();
void *GC_realloc();
char *GC_strdup();
long GC_get_gc_no();
size_t GC_get_memory_use();
size_t GC_get_free_bytes();
#else
void GC_INIT(void);
void GC_gcollect(void);
void *GC_malloc_atomic(size_t size);
void *GC_malloc(size_t size);
void *GC_realloc(void *old, size_t size);
char *GC_strdup(const char *str);
long GC_get_gc_no(void);		/* LCOV_EXCL_LINE */
size_t GC_get_memory_use(void);		/* LCOV_EXCL_LINE */
size_t GC_get_free_bytes(void);		/* LCOV_EXCL_LINE */
#endif
#endif
