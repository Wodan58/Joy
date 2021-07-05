/*
    module  : fputchars.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FPUTCHARS_C
#define FPUTCHARS_C

/**
1970  fputchars  :  S "abc.."  ->  S
The string abc.. (no quotes) is written to the current position of stream S.
*/
PRIVATE void fputchars_(
    pEnv env) /* suggested by Heiko Kuhrt, as "fputstring_" */
{
    FILE *stm;

    TWOPARAMS("fputchars");
    if (nodetype(nextnode1(env->stck)) != FILE_
        || nodevalue(nextnode1(env->stck)).fil == NULL)
        execerror(env, "file", "fputchars");
    stm = nodevalue(nextnode1(env->stck)).fil;
    fprintf(stm, "%s", nodevalue(env->stck).str);
    POP(env->stck);
}
#endif
