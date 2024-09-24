/*
    module  : fclose.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef FCLOSE_C
#define FCLOSE_C

/**
Q0  OK  1830  fclose  :  S  ->
[FOREIGN] Stream S is closed and removed from the stack.
*/
void fclose_(pEnv env)
{
    ONEPARAM("fclose");
    ISFILE("fclose");
    fclose(nodevalue(env->stck).fil);
    POP(env->stck);
}
#endif
