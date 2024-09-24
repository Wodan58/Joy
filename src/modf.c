/*
    module  : modf.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef MODF_C
#define MODF_C

/**
Q0  OK  1620  modf  :  F  ->  G H
G is the fractional part and H is the integer part
(but expressed as a float) of F.
*/
void modf_(pEnv env)
{
    double exp;

    ONEPARAM("modf");
    FLOAT("modf");
    UNARY(FLOAT_NEWNODE, modf(FLOATVAL, &exp));
    NULLARY(FLOAT_NEWNODE, exp);
}
#endif
