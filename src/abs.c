/*
    module  : abs.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef ABS_C
#define ABS_C

/**
1480  abs  :  N1  ->  N2
Integer N2 is the absolute value (0,1,2..) of integer N1,
or float N2 is the absolute value (0.0 ..) of float N1.
*/
PRIVATE void abs_(pEnv env)
{
    ONEPARAM("abs");
    /* start new */
    FLOAT("abs");
    if (nodetype(env->stck) == INTEGER_) {
        if (nodevalue(env->stck).num < 0)
            UNARY(INTEGER_NEWNODE, -nodevalue(env->stck).num);
        return;
    }
    /* end new */
    FLOAT_U(fabs);
}
#endif
