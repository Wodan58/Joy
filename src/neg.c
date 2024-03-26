/*
    module  : neg.c
    version : 1.3
    date    : 03/21/24
*/
#ifndef NEG_C
#define NEG_C

/**
OK 1450  neg  :  I  ->  J
Integer J is the negative of integer I.  Also supports float.
*/
void neg_(pEnv env)
{
    ONEPARAM("neg");
    /* start new */
    FLOAT("neg");
    if (nodetype(env->stck) == INTEGER_) {
        if (nodevalue(env->stck).num)
            UNARY(INTEGER_NEWNODE, -nodevalue(env->stck).num);
        return;
    }
    /* end new */
    FLOAT_U(-);
}
#endif
