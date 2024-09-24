/*
    module  : fput.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef FPUT_C
#define FPUT_C

/**
Q0  OK  1940  fput  :  S X  ->  S
[FOREIGN] Writes X to stream S, pops X off stack.
*/
void fput_(pEnv env)
{
    FILE *fp;
    Index node;

    TWOPARAMS("fput");
    node = env->stck;
    POP(env->stck);
    ISFILE("fput");
    fp = nodevalue(env->stck).fil;
    writefactor(env, node, fp);
}
#endif
