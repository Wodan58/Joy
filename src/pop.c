/*
    module  : pop.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef POP_C
#define POP_C

/**
1320  pop  :  X  ->
Removes X from top of the stack.
*/
PRIVATE void pop_(pEnv env)
{
    ONEPARAM("pop");
    POP(env->stck);
}
#endif
