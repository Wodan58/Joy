/*
    module  : fflush.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef FFLUSH_C
#define FFLUSH_C

/**
OK 1860  fflush  :  S  ->  S
[FOREIGN] Flush stream S, forcing all buffered output to be written.
*/
void fflush_(pEnv env)
{
    ONEPARAM("fflush");
    FILE("fflush");
    fflush(nodevalue(env->stck).fil);
}
#endif
