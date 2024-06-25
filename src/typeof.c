/*
    module  : typeof.c
    version : 1.5
    date    : 06/21/24
*/
#ifndef TYPEOF_C
#define TYPEOF_C

/**
OK 3220  typeof  :  X  ->  I
[EXT] Replace X by its type.
*/
void typeof_(pEnv env)
{
    ONEPARAM("typeof");
    UNARY(INTEGER_NEWNODE, nodetype(env->stck));
}
#endif
