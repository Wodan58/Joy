/*
    module  : pop.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef POP_C
#define POP_C

/**
Q0  OK  1320  pop  :  X  ->
Removes X from top of the stack.
*/
void pop_(pEnv env)
{
    ONEPARAM("pop");
    POP(env->stck);
}
#endif
