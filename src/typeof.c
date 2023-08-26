/*
    module  : typeof.c
    version : 1.1
    date    : 08/26/23
*/
#ifndef TYPEOF_C
#define TYPEOF_C

/**
OK 2323  typeof  :  X  ->  I
Replace X by its type.
*/
void typeof_(pEnv env)
{
    ONEPARAM("typeof");
    UNARY(INTEGER_NEWNODE, nodetype(env->stck));
}
#endif
