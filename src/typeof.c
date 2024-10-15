/*
    module  : typeof.c
    version : 1.7
    date    : 09/26/24
*/
#ifndef TYPEOF_C
#define TYPEOF_C

/**
Q0  OK  3220  typeof  :  X  ->  I
Replace X by its type.
*/
void typeof_(pEnv env)
{
    ONEPARAM("typeof");
    UNARY(INTEGER_NEWNODE, nodetype(env->stck));
}
#endif
