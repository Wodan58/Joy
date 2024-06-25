/*
    module  : over.c
    version : 1.5
    date    : 06/21/24
*/
#ifndef OVER_C
#define OVER_C

/**
OK 3180  over  :  X Y  ->  X Y X
[EXT] Pushes an extra copy of the second item X on top of the stack.
*/
void over_(pEnv env)
{
    TWOPARAMS("over");
    GNULLARY(nextnode1(env->stck));
}
#endif
