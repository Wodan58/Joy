/*
    module  : gc.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef GC_C
#define GC_C

/**
OK 3010  gc  :  ->
Initiates garbage collection.
*/
PUBLIC void gc_(pEnv env)
{
    my_gc(env);
}
#endif
