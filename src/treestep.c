/*
    module  : treestep.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef TREESTEP_C
#define TREESTEP_C

/**
Q1  OK  2870  treestep  :  T [P]  ->  ...
Recursively traverses leaves of tree T, executes P for each leaf.
*/
void treestepaux(pEnv env, Index item)
{
    if (nodetype(item) != LIST_) {
	GNULLARY(item);
	exeterm(env, nodevalue(SAVED1).lis);
    } else {
	env->dump1 = LIST_NEWNODE(nodevalue(item).lis, env->dump1);
	for (; DMP1; DMP1 = nextnode1(DMP1))
	    treestepaux(env, DMP1);
	POP(env->dump1);
    }
}

void treestep_(pEnv env)
{
    TWOPARAMS("treestep");
    ONEQUOTE("treestep");
    SAVESTACK;
    env->stck = SAVED3;
    treestepaux(env, SAVED2);
    POP(env->dump);
}
#endif
