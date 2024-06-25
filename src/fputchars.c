/*
    module  : fputchars.c
    version : 1.7
    date    : 06/21/24
*/
#ifndef FPUTCHARS_C
#define FPUTCHARS_C

/**
OK 1960  fputchars  :  S "abc.."  ->  S
[FOREIGN] The string abc.. (no quotes) is written to the current position of
stream S.
*/
void fputchars_(pEnv env) /* suggested by Heiko Kuhrt, as "fputstring_" */
{
    FILE *fp;
    char *str;

    TWOPARAMS("fputchars");
    STRING("fputchars");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    POP(env->stck);
    FILE("fputchars");
    fp = nodevalue(env->stck).fil;
    fprintf(fp, "%s", str);
}
#endif
