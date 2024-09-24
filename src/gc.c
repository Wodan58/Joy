/*
    module  : gc.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef GC_C
#define GC_C

/**
Q0  OK  3010  gc  :  ->
[IMPURE] Initiates garbage collection.
*/
void gc_(pEnv env)
{
    my_gc(env);
}
#endif
