/*
    module  : fputchars.c
    version : 1.9
    date    : 10/18/24
*/
#ifndef FPUTCHARS_C
#define FPUTCHARS_C

/**
Q0  OK  1960  fputchars  :  S "abc.."  ->  S
[FOREIGN] The string abc.. (no quotes) is written to the current position of
stream S.
*/
void fputchars_(pEnv env)	/* suggested by Heiko Kuhrt, as "fputstring_" */
{
    FILE *fp;
    char *str;

    TWOPARAMS("fputchars");
    STRING("fputchars");
    str = GETSTRING(env->stck);
    POP(env->stck);
    ISFILE("fputchars");
    fp = nodevalue(env->stck).fil;
    fprintf(fp, "%s", str);
}
#endif
