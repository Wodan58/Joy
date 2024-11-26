/*
    module  : while.c
    version : 1.10
    date    : 11/11/24
*/
#ifndef WHILE_C
#define WHILE_C

#include "boolean.h"

/**
Q2  OK  2700  while  :  [B] [D]  ->  ...
While executing B yields true executes D.
*/
void while_(pEnv env)
{
    int result;

    TWOPARAMS("while");
    TWOQUOTES("while");
    SAVESTACK;
    while (1) {
	env->stck = SAVED3;
	exeterm(env, nodevalue(SAVED2).lis);	/* TEST */
	CHECKSTACK("while");
	result = get_boolean(env, env->stck);
	if (!result)
	    break;
	env->stck = SAVED3;
	exeterm(env, nodevalue(SAVED1).lis);	/* DO */
	SAVED3 = env->stck;
    }
    env->stck = SAVED3;
    POP(env->dump);
}
#endif
