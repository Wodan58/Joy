/*
    module  : condnestrec.c
    version : 1.10
    date    : 11/11/24
*/
#ifndef CONDNESTREC_C
#define CONDNESTREC_C

#include "boolean.h"

/**
Q1  OK  2750  condnestrec  :  [ [C1] [C2] .. [D] ]  ->  ...
A generalisation of condlinrec.
Each [Ci] is of the form [[B] [R1] [R2] .. [Rn]] and [D] is of the form
[[R1] [R2] .. [Rn]]. Tries each B, or if all fail, takes the default [D].
For the case taken, executes each [Ri] but recurses between any two
consecutive [Ri] (n > 3 would be exceptional.)
*/
void condnestrecaux(pEnv env)
{
    int result = 0;

    env->dump1 = LIST_NEWNODE(nodevalue(SAVED1).lis, env->dump1);
    env->dump2 = LIST_NEWNODE(env->stck, env->dump2);
    for (; DMP1 && nextnode1(DMP1); DMP1 = nextnode1(DMP1)) {
	env->stck = DMP2;
	exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
	result = get_boolean(env, env->stck);
	if (result)
	    break;
    }
    env->stck = DMP2;
    env->dump3 = LIST_NEWNODE(
	(result ? nextnode1(nodevalue(DMP1).lis) : nodevalue(DMP1).lis),
	env->dump3);
    exeterm(env, nodevalue(DMP3).lis);
    for (DMP3 = nextnode1(DMP3); DMP3; DMP3 = nextnode1(DMP3)) {
	condnestrecaux(env);
	exeterm(env, nodevalue(DMP3).lis);
    }
    POP(env->dump3);
    POP(env->dump2);
    POP(env->dump1);
}

void condnestrec_(pEnv env)
{
    ONEPARAM("condnestrec");
    LIST("condnestrec");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "condnestrec");
    SAVESTACK;
    env->stck = SAVED2;
    condnestrecaux(env);
    POP(env->dump);
}
#endif
