/*
    module  : trunc.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef TRUNC_C
#define TRUNC_C

/**
Q0  OK  1690  trunc  :  F  ->  I
I is an integer equal to the float F truncated toward zero.
*/
void trunc_(pEnv env)
{
    ONEPARAM("trunc");
    FLOAT("trunc");
    UNARY(INTEGER_NEWNODE, (int64_t)FLOATVAL);
}
#endif
