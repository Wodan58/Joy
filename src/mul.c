/*
    module  : mul.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef MUL_C
#define MUL_C

/**
1400  *  :  I J  ->  K
Integer K is the product of integers I and J.  Also supports float.
*/
PRIVATE void mul_(pEnv env)
{
    TWOPARAMS("*");
    FLOAT_I(*);
    INTEGERS2("*");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num * nodevalue(env->stck).num);
}
#endif
