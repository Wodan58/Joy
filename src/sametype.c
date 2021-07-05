/*
    module  : sametype.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef SAMETYPE_C
#define SAMETYPE_C

/**
2320  sametype  :  X Y  ->  B
Tests whether X and Y have the same type.
*/
PRIVATE void sametype_(pEnv env)
{
    TWOPARAMS("sametype");
    BINARY(
        BOOLEAN_NEWNODE, nodetype(env->stck) == nodetype(nextnode1(env->stck)));
}
#endif
