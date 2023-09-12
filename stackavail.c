/*
    module  : stackavail.c
    version : 1.3
    date    : 09/12/23
*/
#include <stddef.h>

#ifdef WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/resource.h>

extern char *bottom_of_stack;	/* cold end of the stack, from main() */
#endif

/* protect against stack overflow */
size_t stackavail(void)
{
#ifdef WIN32
    MEMORY_BASIC_INFORMATION mbi;

    VirtualQuery((PVOID)&mbi, &mbi, sizeof(mbi));
    return (UINT_PTR)&mbi-(UINT_PTR)mbi.AllocationBase;
#else
    struct rlimit limit;

    getrlimit(RLIMIT_STACK, &limit);
/*
    &limit = top_of_stack, at a lower address than bottom_of_stack
    1. bottom_of_stack - &limit = number of bytes that are in use
    2. limit.rlim_cur = currently maximum available stack space
    3. 1) subtracted from 2) is the remaining available stack space
    The (char *) casts assume that the stack is a character array.
*/
    return limit.rlim_cur - (bottom_of_stack - (char *)&limit);    
#endif
}
