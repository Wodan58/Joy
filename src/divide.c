/*
    module  : divide.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef DIVIDE_C
#define DIVIDE_C

/**
1410  /  :  I J  ->  K
Integer K is the (rounded) ratio of integers I and J.  Also supports float.
*/
PRIVATE void divide_(pEnv env)
{
    TWOPARAMS("/");
    if ((nodetype(env->stck) == FLOAT_ && nodevalue(env->stck).dbl == 0.0)
        || (nodetype(env->stck) == INTEGER_ && nodevalue(env->stck).num == 0))
        execerror(env, "non-zero divisor", "/");
    FLOAT_I(/);
    INTEGERS2("/");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num / nodevalue(env->stck).num);
}
#endif
