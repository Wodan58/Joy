/*
    module  : unstack.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef UNSTACK_C
#define UNSTACK_C

/**
OK 2010  unstack  :  [X Y ..]  ->  ..Y X
The list [X Y ..] becomes the new stack.
*/
PRIVATE void unstack_(pEnv env)
{
    ONEPARAM("unstack");
    LIST("unstack");
    env->stck = nodevalue(env->stck).lis;
}
#endif
