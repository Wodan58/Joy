/*
    module  : sametype.c
    version : 1.3
    date    : 08/26/23
*/
#ifndef SAMETYPE_C
#define SAMETYPE_C

/**
OK 2320  sametype  :  X Y  ->  B
Tests whether X and Y have the same type.
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
