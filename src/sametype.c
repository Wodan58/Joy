/*
    module  : sametype.c
    version : 1.9
    date    : 09/26/24
*/
#ifndef SAMETYPE_C
#define SAMETYPE_C

/**
Q0  OK  3210  sametype  :  X Y  ->  B
Tests whether X and Y have the same type.
*/
void sametype_(pEnv env)
{
    int op;

    TWOPARAMS("sametype");
    if ((op = nodetype(env->stck)) == ANON_FUNCT_)
	op = nodevalue(env->stck).proc == nodevalue(nextnode1(env->stck)).proc;
    else
	op = op == nodetype(nextnode1(env->stck));
    BINARY(BOOLEAN_NEWNODE, op);
}
#endif
