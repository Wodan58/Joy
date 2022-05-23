/*
    module  : div.c
    version : 1.2
    date    : 05/17/22
*/
#ifndef DIV_C
#define DIV_C

/**
1430  div  :  I J  ->  K L
Integers K and L are the quotient and remainder of dividing I by J.
*/
PRIVATE void div_(pEnv env)
{
    ldiv_t result;

    TWOPARAMS("div");
    INTEGERS2("div");
    CHECKZERO("div");
    result
        = ldiv(nodevalue(nextnode1(env->stck)).num, nodevalue(env->stck).num);
    BINARY(INTEGER_NEWNODE, result.quot);
    NULLARY(INTEGER_NEWNODE, result.rem);
}
#endif
