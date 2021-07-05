/*
    module  : trunc.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef TRUNC_C
#define TRUNC_C

/**
1690  trunc  :  F  ->  I
I is an integer equal to the float F truncated toward zero.
*/
PRIVATE void trunc_(pEnv env)
{
    ONEPARAM("trunc");
    FLOAT("trunc");
    UNARY(INTEGER_NEWNODE, (long_t)FLOATVAL);
}
#endif
