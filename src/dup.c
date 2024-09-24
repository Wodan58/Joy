/*
    module  : dup.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef DUP_C
#define DUP_C

/**
Q0  OK  1210  dup  :  X  ->  X X
Pushes an extra copy of X onto stack.
*/
void dup_(pEnv env)
{
    ONEPARAM("dup");
    GNULLARY(env->stck);
}
#endif
