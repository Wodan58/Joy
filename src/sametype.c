/*
    module  : sametype.c
    version : 1.7
    date    : 03/21/24
*/
#ifndef SAMETYPE_C
#define SAMETYPE_C

/**
OK 3210  sametype  :  X Y  ->  B
[EXT] Tests whether X and Y have the same type.
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
