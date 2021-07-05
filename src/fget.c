/*
    module  : fget.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FGET_C
#define FGET_C

/**
1870  fget  :  S  ->  S F
Reads a factor from stream S and pushes it onto stack.
*/
PRIVATE void fget_(pEnv env)
{
    FILE *stm;

    ONEPARAM("fget");
    if (nodetype(env->stck) != FILE_ || nodevalue(env->stck).fil == NULL)
        execerror(env, "file", "fget");
    stm = nodevalue(env->stck).fil;
    redirect(env, stm);
    getsym(env);
    readfactor(env, 0);
}
#endif
