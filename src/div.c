/*
    module  : div.c
    version : 1.6
    date    : 07/12/24
*/
#ifndef DIV_C
#define DIV_C

/**
OK 1430  div  :  I J  ->  K L
Integers K and L are the quotient and remainder of dividing I by J.
*/
void div_(pEnv env)
{
    lldiv_t rv;

    TWOPARAMS("div");
    INTEGERS2("div");
    CHECKZERO("div");
    rv.quot = nodevalue(nextnode1(env->stck)).num / nodevalue(env->stck).num;
    rv.rem = nodevalue(nextnode1(env->stck)).num % nodevalue(env->stck).num;
    BINARY(INTEGER_NEWNODE, rv.quot);
    NULLARY(INTEGER_NEWNODE, rv.rem);
}
#endif
