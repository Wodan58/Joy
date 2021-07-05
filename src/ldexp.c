/*
    module  : ldexp.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef LDEXP_C
#define LDEXP_C

/**
1590  ldexp  :  F I  ->  G
G is F times 2 to the Ith power.
*/
PRIVATE void ldexp_(pEnv env)
{
    int exp;

    TWOPARAMS("ldexp");
    INTEGER("ldexp");
    exp = nodevalue(env->stck).num;
    POP(env->stck);
    FLOAT("ldexp");
    UNARY(FLOAT_NEWNODE, ldexp(FLOATVAL, exp));
}
#endif
