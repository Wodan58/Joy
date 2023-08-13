/*
    module  : fputchars.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef FPUTCHARS_C
#define FPUTCHARS_C

/**
OK 1970  fputchars  :  S "abc.."  ->  S
The string abc.. (no quotes) is written to the current position of stream S.
*/
PRIVATE void fputchars_(
    pEnv env) /* suggested by Heiko Kuhrt, as "fputstring_" */
{
    FILE *fp;
    char *str;

    TWOPARAMS("fputchars");
    STRING("fputchars");
    str = nodevalue(env->stck).str;
    POP(env->stck);
    FILE("fputchars");
    fp = nodevalue(env->stck).fil;
    fprintf(fp, "%s", str);
}
#endif
