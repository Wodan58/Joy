/*
    module  : ifte.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef IFTE_C
#define IFTE_C

/**
2620  ifte  :  [B] [T] [F]  ->  ...
Executes B. If that yields true, then executes T else executes F.
*/
PRIVATE void ifte_(pEnv env)
{
    int result;
    THREEPARAMS("ifte");
    THREEQUOTES("ifte");
    SAVESTACK;
    env->stck = SAVED4;
    exeterm(env, nodevalue(SAVED3).lis);
    result = nodevalue(env->stck).num;
    env->stck = SAVED4;
    exeterm(env, result ? nodevalue(SAVED2).lis : nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
