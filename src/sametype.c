/*
    module  : sametype.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef SAMETYPE_C
#define SAMETYPE_C

/**
OK 3240  sametype  :  X Y  ->  B
[EXT] Tests whether X and Y have the same type.
*/
PRIVATE void sametype_(pEnv env)
{
    int op;

    TWOPARAMS("sametype");
    if ((op = nodetype(env->stck)) == ANON_FUNCT_)
	op = nodevalue(env->stck).lis == nodevalue(nextnode1(env->stck)).lis;
    else
	op = op == nodetype(nextnode1(env->stck));
    BINARY(BOOLEAN_NEWNODE, op);
}
#endif
