/*
    module  : trunc.c
    version : 1.2
    date    : 05/17/22
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
    UNARY(INTEGER_NEWNODE, (long)FLOATVAL);
}
#endif
