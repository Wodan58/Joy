/*
    module  : casting.c
    version : 1.1
    date    : 08/26/23
*/
#ifndef CASTING_C
#define CASTING_C

/**
OK 2326  casting  :  DDA 	X Y  ->  Z
Z takes the value from X and the type from Y.
*/
void casting_(pEnv env)
{
    TWOPARAMS("casting");
    GBINARY(nodetype(env->stck), nodevalue(nextnode1(env->stck)));
}
#endif
