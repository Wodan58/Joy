/*
    module  : divide.c
    version : 1.4
    date    : 08/13/23
*/
#ifndef DIVIDE_C
#define DIVIDE_C

/**
OK 1410  /\0divide  :  I J  ->  K
Integer K is the (rounded) ratio of integers I and J.  Also supports float.
*/
PRIVATE void divide_(pEnv env)
{
    TWOPARAMS("/");
    CHECKDIVISOR("/");
    FLOAT_I(/);
    INTEGERS2("/");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num / nodevalue(env->stck).num);
}
#endif
