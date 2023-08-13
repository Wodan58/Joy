/*
    module  : dup.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef DUP_C
#define DUP_C

/**
OK 1210  dup  :  X  ->  X X
Pushes an extra copy of X onto stack.
*/
PRIVATE void dup_(pEnv env)
{
    ONEPARAM("dup");
    GNULLARY(nodetype(env->stck), nodevalue(env->stck));
}
#endif
