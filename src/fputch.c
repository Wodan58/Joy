/*
    module  : fputch.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef FPUTCH_C
#define FPUTCH_C

/**
Q0  OK  1950  fputch  :  S C  ->  S
[FOREIGN] The character C is written to the current position of stream S.
*/
void fputch_(pEnv env)
{
    int ch;

    TWOPARAMS("fputch");
    NUMERICTYPE("fputch");
    ch = nodevalue(env->stck).num;
    POP(env->stck);
    ISFILE("fputch");
    putc(ch, nodevalue(env->stck).fil);
}
#endif
