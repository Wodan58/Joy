/*
    module  : modf.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef MODF_C
#define MODF_C

/**
OK 1620  modf  :  F  ->  G H
G is the fractional part and H is the integer part
(but expressed as a float) of F.
*/
PRIVATE void modf_(pEnv env)
{
    double exp;

    ONEPARAM("modf");
    FLOAT("modf");
    UNARY(FLOAT_NEWNODE, modf(FLOATVAL, &exp));
    NULLARY(FLOAT_NEWNODE, exp);
}
#endif
