/*
    module  : equal.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef EQUAL_C
#define EQUAL_C

#include "compare.h"

/**
Q0  OK  2280  equal  :  T U  ->  B
(Recursively) tests whether trees T and U are identical.
*/
static int equal_aux(pEnv env, Index n1, Index n2); /* forward */

static int equal_list_aux(pEnv env, Index n1, Index n2)
{
    if (!n1 && !n2)
	return 1;
    if (!n1 || !n2)
	return 0;
    if (equal_aux(env, n1, n2))
	return equal_list_aux(env, nextnode1(n1), nextnode1(n2));
    else
	return 0;
}

static int equal_aux(pEnv env, Index n1, Index n2)
{
    if (nodetype(n1) == LIST_ && nodetype(n2) == LIST_)
	return equal_list_aux(env, nodevalue(n1).lis, nodevalue(n2).lis);
    return !Compare(env, n1, n2);
}

void equal_(pEnv env)
{
    TWOPARAMS("equal");
    BINARY(BOOLEAN_NEWNODE, equal_aux(env, env->stck, nextnode1(env->stck)));
}
#endif
