/*
    module  : divide.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef DIVIDE_C
#define DIVIDE_C

/**
Q0  OK  1410  /\0divide  :  I J  ->  K
Integer K is the (rounded) ratio of integers I and J.  Also supports float.
*/
void divide_(pEnv env)
{
    TWOPARAMS("divide");
    CHECKDIVISOR("divide");
    FLOAT_I(/);
    INTEGERS2("divide");
    BINARY(INTEGER_NEWNODE, nodevalue(nextnode1(env->stck)).num /
		    nodevalue(env->stck).num);
}
#endif
