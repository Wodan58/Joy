/*
    module  : fputch.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FPUTCH_C
#define FPUTCH_C

/**
1960  fputch  :  S C  ->  S
The character C is written to the current position of stream S.
*/
PRIVATE void fputch_(pEnv env)
{
    int ch;

    TWOPARAMS("fputch");
    INTEGER("fputch");
    ch = nodevalue(env->stck).num;
    POP(env->stck);
    FILE("fputch");
    putc(ch, nodevalue(env->stck).fil);
}
#endif
