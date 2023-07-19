/*
    module  : fput.c
    version : 1.5
    date    : 07/18/23
*/
#ifndef FPUT_C
#define FPUT_C

/**
1950  fput  :  S X  ->  S
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
    fflush(stdout);
    fp = nodevalue(env->stck).fil;
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
