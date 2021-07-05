/*
    module  : fclose.c
    version : 1.1
    date    : 05/21/21
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
    if (nodetype(env->stck) == FILE_ && nodevalue(env->stck).fil == NULL) {
        POP(env->stck);
        return;
    }
    FILE("fclose");
    fclose(nodevalue(env->stck).fil);
    POP(env->stck);
}
#endif
