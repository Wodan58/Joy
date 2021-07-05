/*
    module  : %M%
    version : %I%
    date    : %G%
*/
#ifndef EQUAL_C
#define EQUAL_C

#include "compare.h"

PRIVATE int equal_aux(pEnv env, Index n1, Index n2); /* forward */

PRIVATE int equal_list_aux(pEnv env, Index n1, Index n2)
{
    if (n1 == NULL && n2 == NULL)
        return 1;
    if (n1 == NULL || n2 == NULL)
        return 0;
    if (equal_aux(env, n1, n2))
        return equal_list_aux(env, nextnode1(n1), nextnode1(n2));
    else
        return 0;
}

PRIVATE int equal_aux(pEnv env, Index n1, Index n2)
{
    int error;

    if (n1 == NULL && n2 == NULL)
        return 1;
    if (n1 == NULL || n2 == NULL)
        return 0;
    if (nodetype(n1) == LIST_ && nodetype(n2) == LIST_)
        return equal_list_aux(env, nodevalue(n1).lis, nodevalue(n2).lis);
    return !Compare(env, n1, n2, &error) && !error;
}

/**
2290  equal  :  T U  ->  B
(Recursively) tests whether trees T and U are identical.
*/
PRIVATE void equal_(pEnv env)
{
    TWOPARAMS("equal");
    BINARY(BOOLEAN_NEWNODE, equal_aux(env, env->stck, nextnode1(env->stck)));
}
#endif
