/*
    module  : fput.c
    version : 1.7
    date    : 01/17/24
*/
#ifndef FPUT_C
#define FPUT_C

/**
OK 1940  fput  :  S X  ->  S
[FOREIGN] Writes X to stream S, pops X off stack.
*/
PRIVATE void fput_(pEnv env)
{
    FILE *fp;
    Index node;

    TWOPARAMS("fput");
    node = env->stck;
    POP(env->stck);
    FILE("fput");
    fp = nodevalue(env->stck).fil;
    writefactor(env, node, fp);
    putc(' ', fp);
}
#endif
