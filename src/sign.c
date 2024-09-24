/*
    module  : sign.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef SIGN_C
#define SIGN_C

/**
Q0  OK  1440  sign  :  N1  ->  N2
Integer N2 is the sign (-1 or 0 or +1) of integer N1,
or float N2 is the sign (-1.0 or 0.0 or 1.0) of float N1.
*/
void sign_(pEnv env)
{
    double dbl;

    ONEPARAM("sign");
    /* start new */
    FLOAT("sign");
    if (nodetype(env->stck) == INTEGER_) {
	if (nodevalue(env->stck).num != 0 && nodevalue(env->stck).num != 1)
	    UNARY(INTEGER_NEWNODE, nodevalue(env->stck).num > 0 ? 1 : -1);
	return;
    }
    /* end new */
    if (FLOATABLE) {
	dbl = FLOATVAL;
	if (dbl > 0.0)
	    dbl = 1.0;
	else if (dbl < 0.0)
	    dbl = -1.0;
	else
	    dbl = 0.0;
	UNARY(FLOAT_NEWNODE, dbl);
    }
}
#endif
