/*
    module  : fput.c
    version : 1.9
    date    : 04/27/24
*/
#ifndef FPUT_C
#define FPUT_C

/**
OK 1940  fput  :  S X  ->  S
[FOREIGN] Writes X to stream S, pops X off stack.
*/
void fput_(pEnv env)
{
    FILE *fp;
    Index node;

    TWOPARAMS("fput");
    node = env->stck;
    POP(env->stck);
    FILE("fput");
    fp = nodevalue(env->stck).fil;
    writefactor(env, node, fp);
}
#endif
