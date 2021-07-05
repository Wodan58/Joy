/*
    module  : gc.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef GC_C
#define GC_C

/**
3030  gc  :  ->
Initiates garbage collection.
*/
PUBLIC void gc_(pEnv env)
{
    my_gc(env);
}
#endif
