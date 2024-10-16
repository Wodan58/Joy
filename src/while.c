/*
    module  : while.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef WHILE_C
#define WHILE_C

/**
Q2  OK  2700  while  :  [B] [D]  ->  ...
While executing B yields true executes D.
*/
void while_(pEnv env)
{
    TWOPARAMS("while");
    TWOQUOTES("while");
    SAVESTACK;
    while (1) {
	env->stck = SAVED3;
	exeterm(env, nodevalue(SAVED2).lis); /* TEST */
	CHECKSTACK("while");
	if (!nodevalue(env->stck).num)
	    break;
	env->stck = SAVED3;
	exeterm(env, nodevalue(SAVED1).lis); /* DO */
	SAVED3 = env->stck;
    }
    env->stck = SAVED3;
    POP(env->dump);
}
#endif
