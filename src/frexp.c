/*
    module  : frexp.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FREXP_C
#define FREXP_C

/**
1580  frexp  :  F  ->  G I
G is the mantissa and I is the exponent of F.
Unless F = 0, 0.5 <= abs(G) < 1.0.
*/
PRIVATE void frexp_(pEnv env)
{
    int exp;

    ONEPARAM("frexp");
    FLOAT("frexp");
    UNARY(FLOAT_NEWNODE, frexp(FLOATVAL, &exp));
    NULLARY(INTEGER_NEWNODE, exp);
}
#endif
