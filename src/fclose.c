/*
    module  : fclose.c
    version : 1.2
    date    : 05/02/22
*/
#ifndef FCLOSE_C
#define FCLOSE_C

/**
1830  fclose  :  S  ->
Stream S is closed and removed from the stack.
*/
PRIVATE void fclose_(pEnv env)
{
    ONEPARAM("fclose");
    FILE("fclose");
    fclose(nodevalue(env->stck).fil);
    POP(env->stck);
}
#endif
