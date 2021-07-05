/*
    module  : fput.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FPUT_C
#define FPUT_C

/**
1950  fput  :  S X  ->  S
Writes X to stream S, pops X off stack.
*/
PRIVATE void fput_(pEnv env)
{
    FILE *stm;

    TWOPARAMS("fput");
    if (nodetype(nextnode1(env->stck)) != FILE_
        || nodevalue(nextnode1(env->stck)).fil == NULL)
        execerror(env, "file", "fput");
    stm = nodevalue(nextnode1(env->stck)).fil;
    writefactor(env, env->stck, stm);
    fprintf(stm, " ");
    POP(env->stck);
}
#endif
