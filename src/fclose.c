/*
    module  : fclose.c
    version : 1.5
    date    : 03/21/24
*/
#ifndef FCLOSE_C
#define FCLOSE_C

/**
OK 1830  fclose  :  S  ->
[FOREIGN] Stream S is closed and removed from the stack.
*/
void fclose_(pEnv env)
{
    ONEPARAM("fclose");
    FILE("fclose");
    fclose(nodevalue(env->stck).fil);
    POP(env->stck);
}
#endif
