/*
    module  : mul.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef MUL_C
#define MUL_C

/**
OK 1400  *\0ast  :  I J  ->  K
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
