/*
    module  : casting.c
    version : 1.4
    date    : 02/01/24
*/
#ifndef CASTING_C
#define CASTING_C

/**
OK 3140  casting  :  X Y  ->  Z
[EXT] Z takes the value from X and uses the value from Y as its type.
*/
void casting_(pEnv env)
{
    Operator op;

    TWOPARAMS("casting");
    op = nodevalue(env->stck).num;
    GBINARY(op, nodevalue(nextnode1(env->stck)));
}
#endif
