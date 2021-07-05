/*
    module  : unstack.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef UNSTACK_C
#define UNSTACK_C

/**
2010  unstack  :  [X Y ..]  ->  ..Y X
The list [X Y ..] becomes the new stack.
*/
PRIVATE void unstack_(pEnv env)
{
    ONEPARAM("unstack");
    LIST("unstack");
    env->stck = nodevalue(env->stck).lis;
}
#endif
