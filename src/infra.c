/*
    module  : infra.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef INFRA_C
#define INFRA_C

/**
Q1  OK  2810  infra  :  L1 [P]  ->  L2
Using list L1 as stack, executes P and returns a new list L2.
The first element of L1 is used as the top of stack,
and after execution of P the top of stack becomes the first element of L2.
*/
void infra_(pEnv env)
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
