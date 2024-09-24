/*
    module  : step.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef STEP_C
#define STEP_C

/**
Q1  OK  2770  step  :  A [P]  ->  ...
Sequentially putting members of aggregate A onto stack,
executes P for each member of A.
*/
void step_(pEnv env)
{
    int i = 0;
    char *str;

    TWOPARAMS("step");
    ONEQUOTE("step");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    switch (nodetype(SAVED2)) {
    case LIST_:
	env->dump1 = LIST_NEWNODE(nodevalue(SAVED2).lis, env->dump1);
	for (; DMP1; DMP1 = nextnode1(DMP1)) {
	    GNULLARY(DMP1);
	    exeterm(env, nodevalue(SAVED1).lis);
	}
	POP(env->dump1);
	break;
    case STRING_:
	for (str = strdup((char *)&nodevalue(SAVED2)); str[i]; i++) {
	    NULLARY(CHAR_NEWNODE, str[i]);
	    exeterm(env, nodevalue(SAVED1).lis);
	}
	free(str);
	break;
    case SET_:
	for (; i < SETSIZE; i++)
	    if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {
		NULLARY(INTEGER_NEWNODE, i);
		exeterm(env, nodevalue(SAVED1).lis);
	    }
	break;
    default:
	BADAGGREGATE("step");
    }
    POP(env->dump);
}
#endif
