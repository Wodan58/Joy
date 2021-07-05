/*
    module  : dip.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef DIP_C
#define DIP_C

/**
2450  dip  :  X [P]  ->  ...  X
Saves X, executes P, pushes X back.
*/
PRIVATE void dip_(pEnv env)
{
    TWOPARAMS("dip");
    ONEQUOTE("dip");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}
#endif
