/*
    module  : sign.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef SIGN_C
#define SIGN_C

PRIVATE double fsgn(double f)
{
    if (f < 0)
        return -1.0;
    if (f > 0)
        return 1.0;
    return 0.0;
}

/**
1440  sign  :  N1  ->  N2
Integer N2 is the sign (-1 or 0 or +1) of integer N1,
or float N2 is the sign (-1.0 or 0.0 or 1.0) of float N1.
*/
PRIVATE void sign_(pEnv env)
{
    ONEPARAM("sign");
    /* start new */
    FLOAT("sign");
    if (nodetype(env->stck) == INTEGER_) {
        if (nodevalue(env->stck).num != 0 && nodevalue(env->stck).num != 1)
            UNARY(INTEGER_NEWNODE, nodevalue(env->stck).num > 0 ? 1 : -1);
        return;
    }
    /* end new */
    FLOAT_U(fsgn);
}
#endif
