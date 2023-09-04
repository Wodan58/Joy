/*
    module  : over.c
    version : 1.2
    date    : 09/04/23
*/
#ifndef OVER_C
#define OVER_C

/**
OK 3210  over  :  X Y  ->  X Y X
[EXT] Pushes an extra copy of the second item X on top of the stack.
*/
void over_(pEnv env)
{
    TWOPARAMS("over");
    GNULLARY(nodetype(nextnode1(env->stck)), nodevalue(nextnode1(env->stck)));
}
#endif
