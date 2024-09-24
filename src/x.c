/*
    module  : x.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef X_C
#define X_C

/**
Q1  OK  2410  x  :  [P] x  ->  ...
Executes P without popping [P]. So, [P] x  ==  [P] P.
*/
void x_(pEnv env)
{
    ONEPARAM("x");
    ONEQUOTE("x");
    exeterm(env, nodevalue(env->stck).lis);
}
#endif
