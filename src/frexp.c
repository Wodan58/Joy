/*
    module  : frexp.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef FREXP_C
#define FREXP_C

/**
Q0  OK  1580  frexp  :  F  ->  G I
G is the mantissa and I is the exponent of F.
Unless F = 0, 0.5 <= abs(G) < 1.0.
*/
void frexp_(pEnv env)
{
    int exp;

    ONEPARAM("frexp");
    FLOAT("frexp");
    UNARY(FLOAT_NEWNODE, frexp(FLOATVAL, &exp));
    NULLARY(INTEGER_NEWNODE, exp);
}
#endif
