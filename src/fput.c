/*
    module  : fput.c
    version : 1.4
    date    : 08/26/23
*/
#ifndef FPUT_C
#define FPUT_C

/**
OK 1950  fput  :  S X  ->  S
Writes X to stream S, pops X off stack.
*/
PRIVATE void fput_(pEnv env)
{
    FILE *fp;
    Index node;
    int stdout_dup;

    TWOPARAMS("fput");
    node = env->stck;
    POP(env->stck);
    FILE("fput");
    fp = nodevalue(env->stck).fil;
    fflush(stdout);
    if ((stdout_dup = dup(1)) != -1)
        dup2(fileno(fp), 1);
    writefactor(env, node);
    putchar(' ');
    fflush(stdout);
    if (stdout_dup != -1) {
        dup2(stdout_dup, 1);
        close(stdout_dup);
    }
}
#endif
