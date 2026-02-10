/*
    module  : casting.c
    version : 1.6
    date    : 02/04/26
*/
#ifndef CASTING_C
#define CASTING_C

/**
Q0  OK  3150  casting  :  X Y  ->  Z
Z takes the value from X and uses the value from Y as its type.
*/
void casting_(pEnv env)
{
    Node node;

    TWOPARAMS("casting");
    node.op = nodevalue(env->stck).num;
    POP(env->stck);
    if (node.op == STRING_ || node.op == BIGNUM_)
	node.u.str = GC_strdup((char *)&nodevalue(env->stck));
    else
	node.u = nodevalue(env->stck);
    env->stck = newnode(env, node.op, node.u, nextnode1(env->stck));
}
#endif
