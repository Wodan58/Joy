/*
    module  : pop.c
    version : 1.3
    date    : 03/21/24
*/
#ifndef POP_C
#define POP_C

/**
OK 1320  pop  :  X  ->
Removes X from top of the stack.
*/
void pop_(pEnv env)
{
    ONEPARAM("pop");
    POP(env->stck);
}
#endif
