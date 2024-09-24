/*
    module  : fflush.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef FFLUSH_C
#define FFLUSH_C

/**
Q0  OK  1860  fflush  :  S  ->  S
[FOREIGN] Flush stream S, forcing all buffered output to be written.
*/
void fflush_(pEnv env)
{
    ONEPARAM("fflush");
    ISFILE("fflush");
    fflush(nodevalue(env->stck).fil);
}
#endif
