/*
    module  : unstack.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef UNSTACK_C
#define UNSTACK_C

/**
Q0  OK  2000  unstack  :  [X Y ..]  ->  ..Y X
The list [X Y ..] becomes the new stack.
*/
void unstack_(pEnv env)
{
    ONEPARAM("unstack");
    LIST("unstack");
    env->stck = nodevalue(env->stck).lis;
}
#endif
