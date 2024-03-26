/*
    module  : gc.c
    version : 1.5
    date    : 03/21/24
*/
#ifndef GC_C
#define GC_C

/**
OK 3010  gc  :  ->
[IMPURE] Initiates garbage collection.
*/
void gc_(pEnv env)
{
    my_gc(env);
}
#endif
