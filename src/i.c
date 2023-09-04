/*
    module  : i.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef I_C
#define I_C

/**
OK 2410  i  :  [P]  ->  ...
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
