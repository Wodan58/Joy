/*
    module  : i.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef I_C
#define I_C

/**
2430  i  :  [P]  ->  ...
Executes P. So, [P] i  ==  P.
*/
PRIVATE void i_(pEnv env)
{
    ONEPARAM("i");
    ONEQUOTE("i");
    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
