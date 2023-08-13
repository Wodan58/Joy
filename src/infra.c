/*
    module  : infra.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef INFRA_C
#define INFRA_C

/**
OK 2830  infra  :  L1 [P]  ->  L2
Using list L1 as stack, executes P and returns a new list L2.
The first element of L1 is used as the top of stack,
and after execution of P the top of stack becomes the first element of L2.
*/
PRIVATE void infra_(pEnv env)
{
    TWOPARAMS("infra");
    ONEQUOTE("infra");
    LIST2("infra");
    SAVESTACK;
    env->stck = nodevalue(SAVED2).lis;
    exeterm(env, nodevalue(SAVED1).lis);
    env->stck = LIST_NEWNODE(env->stck, SAVED3);
    POP(env->dump);
}
#endif
