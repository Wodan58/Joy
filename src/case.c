/*
    module  : case.c
    version : 1.8
    date    : 09/17/24
*/
#ifndef CASE_C
#define CASE_C

#include "compare.h"

/**
Q1  OK  2100  case  :  X [..[X Y]..]  ->  Y i
Indexing on the value of X, execute the matching Y.
*/
void case_(pEnv env)
{
    Index n;

    TWOPARAMS("case");
    LIST("case");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "case");
    while (nextnode1(n) && nodetype(n) == LIST_) {
	if (!Compare(env, nodevalue(n).lis, nextnode1(env->stck)))
	    break;
	n = nextnode1(n);
    }
    CHECKLIST(nodetype(n), "case");
    if (nextnode1(n)) {
	env->stck = nextnode2(env->stck);
	exeterm(env, nextnode1(nodevalue(n).lis));
    } else {
	env->stck = nextnode1(env->stck);
	exeterm(env, nodevalue(n).lis);
    }
}
#endif
