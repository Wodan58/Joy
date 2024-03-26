/*
    module  : unstack.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef UNSTACK_C
#define UNSTACK_C

/**
OK 2000  unstack  :  [X Y ..]  ->  ..Y X
The list [X Y ..] becomes the new stack.
*/
void unstack_(pEnv env)
{
    ONEPARAM("unstack");
    LIST("unstack");
    env->stck = nodevalue(env->stck).lis;
}
#endif
