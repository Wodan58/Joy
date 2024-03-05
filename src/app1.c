/*
    module  : app1.c
    version : 1.4
    date    : 03/05/24
*/
#ifndef APP1_C
#define APP1_C

/**
OK 2440  app1  :  X [P]  ->  R
Obsolescent.  Executes P, pushes result R on stack.
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
