/*
    module  : opcase.c
    version : 1.3
    date    : 05/02/22
*/
#ifndef OPCASE_C
#define OPCASE_C

/**
2100  opcase  :  X [..[X Xs]..]  ->  X [Xs]
Indexing on type of X, returns the list [Xs].
*/
PRIVATE void opcase_(pEnv env)
{
    Index n;

    TWOPARAMS("opcase");
    LIST("opcase");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "opcase");
    while (nextnode1(n) != NULL && nodetype(n) == LIST_
        && nodetype(nodevalue(n).lis) != nodetype(nextnode1(env->stck)))
        n = nextnode1(n);
    CHECKLIST(nodetype(n), "opcase");
    UNARY(LIST_NEWNODE,
        nextnode1(n) != NULL ? nextnode1(nodevalue(n).lis) : nodevalue(n).lis);
}
#endif
