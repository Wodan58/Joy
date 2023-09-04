/*
    module  : x.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef X_C
#define X_C

/**
OK 2410  x  :  [P] x  ->  ...
Executes P without popping [P]. So, [P] x  ==  [P] P.
*/
PRIVATE void x_(pEnv env)
{
    ONEPARAM("x");
    ONEQUOTE("x");
    exeterm(env, nodevalue(env->stck).lis);
}
#endif
