/*
    module  : typeof.c
    version : 1.4
    date    : 02/01/24
*/
#ifndef TYPEOF_C
#define TYPEOF_C

/**
OK 3230  typeof  :  X  ->  I
[EXT] Replace X by its type.
*/
void typeof_(pEnv env)
{
    ONEPARAM("typeof");
    UNARY(INTEGER_NEWNODE, nodetype(env->stck));
}
#endif
