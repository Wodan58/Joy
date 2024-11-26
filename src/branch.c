/*
    module  : branch.c
    version : 1.9
    date    : 11/11/24
*/
#ifndef BRANCH_C
#define BRANCH_C

#include "boolean.h"

/**
Q2  OK  2590  branch  :  B [T] [F]  ->  ...
If B is true, then executes T else executes F.
*/
void branch_(pEnv env)
{
    int result;

    THREEPARAMS("branch");
    TWOQUOTES("branch");
    SAVESTACK;
    env->stck = SAVED4;
    result = get_boolean(env, SAVED3);
    exeterm(env, result ? nodevalue(SAVED2).lis : nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
