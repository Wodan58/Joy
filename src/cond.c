/*
    module  : cond.c
    version : 1.9
    date    : 11/11/24
*/
#ifndef COND_C
#define COND_C

#include "boolean.h"

/**
Q1  OK  2690  cond  :  [..[[Bi] Ti]..[D]]  ->  ...
Tries each Bi. If that yields true, then executes Ti and exits.
If no Bi yields true, executes default D.
*/
void cond_(pEnv env)
{
    Index my_dump;
    int result = 0;

    ONEPARAM("cond");
    LIST("cond");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "cond");
    /* must check for QUOTES in list */
    for (my_dump = nodevalue(env->stck).lis; nextnode1(my_dump);
	 my_dump = nextnode1(my_dump))
	CHECKLIST(nodetype(nodevalue(my_dump).lis), "cond");
    SAVESTACK;
    env->dump1 = LIST_NEWNODE(nodevalue(env->stck).lis, env->dump1);
    for (; DMP1 && nextnode1(DMP1); DMP1 = nextnode1(DMP1)) {
	env->stck = SAVED2;
	exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
	result = get_boolean(env, env->stck);
	if (result)
	    break;
    }
    env->stck = SAVED2;
    if (result)
	exeterm(env, nextnode1(nodevalue(DMP1).lis));
    else
	exeterm(env, nodevalue(DMP1).lis);	/* default */
    POP(env->dump1);
    POP(env->dump);
}
#endif
