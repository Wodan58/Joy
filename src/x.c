/*
    module  : x.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef X_C
#define X_C

/**
2440  x  :  [P] x  ->  ...
Executes P without popping [P]. So, [P] x  ==  [P] P.
*/
PRIVATE void x_(pEnv env)
{
    ONEPARAM("x");
    ONEQUOTE("x");
    exeterm(env, nodevalue(env->stck).lis);
}
#endif
