/*
    module  : dip.c
    version : 1.5
    date    : 06/21/24
*/
#ifndef DIP_C
#define DIP_C

/**
OK 2430  dip  :  X [P]  ->  ...  X
Saves X, executes P, pushes X back.
*/
void dip_(pEnv env)
{
    TWOPARAMS("dip");
    ONEQUOTE("dip");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    GNULLARY(SAVED2);
    POP(env->dump);
}
#endif
