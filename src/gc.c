/*
    module  : gc.c
    version : 1.7
    date    : 01/12/26
*/
#ifndef GC_C
#define GC_C

/**
Q0  IGNORE_OK  3010  gc  :  ->
[IMPURE] Initiates garbage collection.
*/
void gc_(pEnv env)
{
    my_gc(env);
}
#endif
