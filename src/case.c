/*
    module  : case.c
    version : 1.5
    date    : 09/04/23
*/
#ifndef CASE_C
#define CASE_C

#include "compare.h"

/**
OK 2100  case  :  X [..[X Y]..]  ->  Y i
Indexing on the value of X, execute the matching Y.
*/
PRIVATE void case_(pEnv env)
{
    Index n;

    TWOPARAMS("case");
    LIST("case");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "case");
    while (nextnode1(n)) {
        if (!Compare(env, nodevalue(n).lis, nextnode1(env->stck)))
            break;
        n = nextnode1(n);
    }
    if (nextnode1(n)) {
        env->stck = nextnode2(env->stck);
        exeterm(env, nextnode1(nodevalue(n).lis));
    } else {
        env->stck = nextnode1(env->stck);
        exeterm(env, nodevalue(n).lis);
    }
}
#endif
