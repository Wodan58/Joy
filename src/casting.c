/*
    module  : casting.c
    version : 1.2
    date    : 09/04/23
*/
#ifndef CASTING_C
#define CASTING_C

/**
OK 3170  casting  :  X Y  ->  Z
[EXT] Z takes the value from X and the type from Y.
*/
void casting_(pEnv env)
{
    TWOPARAMS("casting");
    GBINARY(nodetype(env->stck), nodevalue(nextnode1(env->stck)));
}
#endif
