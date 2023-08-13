/*
    module  : pop.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef POP_C
#define POP_C

/**
OK 1320  pop  :  X  ->
Removes X from top of the stack.
*/
PRIVATE void pop_(pEnv env)
{
    ONEPARAM("pop");
    POP(env->stck);
}
#endif
