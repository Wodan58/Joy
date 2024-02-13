/*
    module  : gc.c
    version : 1.4
    date    : 01/17/24
*/
#ifndef GC_C
#define GC_C

/**
OK 3010  gc  :  ->
[IMPURE] Initiates garbage collection.
*/
PUBLIC void gc_(pEnv env)
{
    my_gc(env);
}
#endif
