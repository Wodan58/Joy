/*
    module  : fflush.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef FFLUSH_C
#define FFLUSH_C

/**
OK 1860  fflush  :  S  ->  S
Flush stream S, forcing all buffered output to be written.
*/
PRIVATE void fflush_(pEnv env)
{
    ONEPARAM("fflush");
    FILE("fflush");
    fflush(nodevalue(env->stck).fil);
}
#endif
