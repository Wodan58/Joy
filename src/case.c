/*
    module  : case.c
    version : 1.2
    date    : 07/20/21
*/
#ifndef CASE_C
#define CASE_C

#include "compare.h"

/**
2110  case  :  X [..[X Y]..]  ->  Y i
Indexing on the value of X, execute the matching Y.
*/
PRIVATE void case_(pEnv env)
{
    Index n;
    int error;

    TWOPARAMS("case");
    LIST("case");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "case");
    while (nextnode1(n) != NULL
        && nodevalue(nodevalue(n).lis).num
            != nodevalue(nextnode1(env->stck)).num) {
        if (!Compare(env, nodevalue(n).lis, nextnode1(env->stck), &error)
            && !error)
            break;
        n = nextnode1(n);
    }
    if (nextnode1(n) != NULL) {
        env->stck = nextnode2(env->stck);
        exeterm(env, nextnode1(nodevalue(n).lis));
    } else {
        env->stck = nextnode1(env->stck);
        exeterm(env, nodevalue(n).lis);
    }
}
#endif
