/*
    module  : dip.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef DIP_C
#define DIP_C

/**
OK 2430  dip  :  X [P]  ->  ...  X
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
