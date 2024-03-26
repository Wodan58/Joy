/*
    module  : fputch.c
    version : 1.5
    date    : 03/21/24
*/
#ifndef FPUTCH_C
#define FPUTCH_C

/**
OK 1950  fputch  :  S C  ->  S
[FOREIGN] The character C is written to the current position of stream S.
*/
void fputch_(pEnv env)
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
