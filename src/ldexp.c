/*
    module  : ldexp.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef LDEXP_C
#define LDEXP_C

/**
OK 1590  ldexp  :  F I  ->  G
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
