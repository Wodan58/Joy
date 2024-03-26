/*
    module  : dup.c
    version : 1.3
    date    : 03/21/24
*/
#ifndef DUP_C
#define DUP_C

/**
OK 1210  dup  :  X  ->  X X
Pushes an extra copy of X onto stack.
*/
void dup_(pEnv env)
{
    ONEPARAM("dup");
    GNULLARY(nodetype(env->stck), nodevalue(env->stck));
}
#endif
