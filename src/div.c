/*
    module  : div.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef DIV_C
#define DIV_C

/**
Q0  OK  1430  div  :  I J  ->  K L
Integers K and L are the quotient and remainder of dividing I by J.
*/
void div_(pEnv env)
{
    int64_t quotient, remainder;

    TWOPARAMS("div");
    INTEGERS2("div");
    CHECKZERO("div");
    quotient = nodevalue(nextnode1(env->stck)).num / nodevalue(env->stck).num;
    remainder = nodevalue(nextnode1(env->stck)).num % nodevalue(env->stck).num;
    BINARY(INTEGER_NEWNODE, quotient);
    NULLARY(INTEGER_NEWNODE, remainder);
}
#endif
