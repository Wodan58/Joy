/*
    module  : fflush.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FFLUSH_C
#define FFLUSH_C

/**
1860  fflush  :  S  ->  S
Flush stream S, forcing all buffered output to be written.
*/
PRIVATE void fflush_(pEnv env)
{
    ONEPARAM("fflush");
    FILE("fflush");
    fflush(nodevalue(env->stck).fil);
}
#endif
