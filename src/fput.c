/*
    module  : fput.c
    version : 1.2
    date    : 04/11/22
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
        || !nodevalue(nextnode1(env->stck)).fil)
        execerror(env, "file", "fput");
    stm = nodevalue(nextnode1(env->stck)).fil;
    my_writefactor(env, env->stck, stm);
    fputc(' ', stm);
    POP(env->stck);
}
#endif
