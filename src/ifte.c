/*
    module  : ifte.c
    version : 1.8
    date    : 11/11/24
*/
#ifndef IFTE_C
#define IFTE_C

#include "boolean.h"

/**
Q3  OK  2600  ifte  :  [B] [T] [F]  ->  ...
Executes B. If that yields true, then executes T else executes F.
*/
void ifte_(pEnv env)
{
    int result;

    THREEPARAMS("ifte");
    THREEQUOTES("ifte");
    SAVESTACK;
    env->stck = SAVED4;
    exeterm(env, nodevalue(SAVED3).lis);
    result = get_boolean(env, env->stck);
    env->stck = SAVED4;
    exeterm(env, result ? nodevalue(SAVED2).lis : nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
