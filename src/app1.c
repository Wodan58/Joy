/*
    module  : app1.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef APP1_C
#define APP1_C

/**
OK 2440  app1  :  X [P]  ->  R
Executes P, pushes result R on stack.
*/
PRIVATE void app1_(pEnv env)
{
    TWOPARAMS("app1");
    ONEQUOTE("app1");
    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
