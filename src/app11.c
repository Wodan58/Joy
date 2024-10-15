/*
    module  : app11.c
    version : 1.8
    date    : 10/11/24
*/
#ifndef APP11_C
#define APP11_C

#include "i.c"
#include "popd.c"

/**
Q1  OK  2440  app11  :  X Y [P]  ->  R
Executes P, pushes result R on stack.
*/
void app11_(pEnv env)
{
    THREEPARAMS("app11");
    ONEQUOTE("app11");
    i_(env);
    popd_(env);
}
#endif
