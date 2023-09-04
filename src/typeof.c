/*
    module  : typeof.c
    version : 1.2
    date    : 09/04/23
*/
#ifndef TYPEOF_C
#define TYPEOF_C

/**
OK 3250  typeof  :  X  ->  I
[EXT] Replace X by its type.
*/
void typeof_(pEnv env)
{
    ONEPARAM("typeof");
    UNARY(INTEGER_NEWNODE, nodetype(env->stck));
}
#endif
