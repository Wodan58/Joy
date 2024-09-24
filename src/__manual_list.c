/*
    module  : __manual_list.c
    version : 1.8
    date    : 09/17/24
*/
#ifndef __MANUAL_LIST_C
#define __MANUAL_LIST_C

/**
Q0  OK  2960  __manual_list  :  ->  L
Pushes a list L of lists (one per operator) of three documentation strings.
*/
void __manual_list_(pEnv env)
{
    int i;
    Index temp;

    i = sizeof(optable) / sizeof(optable[0]);	/* find end */
    env->dump1 = LIST_NEWNODE(0, env->dump1);
    env->dump2 = LIST_NEWNODE(0, env->dump2);
    while (--i >= 0) {	/* overshot */
	temp = STRING_NEWNODE(optable[i].messg2, 0);
	DMP1 = temp;
	temp = STRING_NEWNODE(optable[i].messg1, DMP1);
	DMP1 = temp;
	temp = STRING_NEWNODE(optable[i].name, DMP1);
	DMP1 = temp;
	temp = LIST_NEWNODE(DMP1, DMP2);
	DMP2 = temp;
    }
    env->stck = LIST_NEWNODE(DMP2, env->stck);
    POP(env->dump2);
    POP(env->dump1);
}
#endif
