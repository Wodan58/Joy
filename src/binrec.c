/*
    module  : binrec.c
    version : 1.9
    date    : 11/11/24
*/
#ifndef BINREC_C
#define BINREC_C

#include "boolean.h"

/**
Q4  OK  2730  binrec  :  [P] [T] [R1] [R2]  ->  ...
Executes P. If that yields true, executes T.
Else uses R1 to produce two intermediates, recurses on both,
then executes R2 to combine their results.
*/
void binrecaux(pEnv env)
{
    int result;

    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED4).lis);
    result = get_boolean(env, env->stck);
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
	exeterm(env, nodevalue(SAVED3).lis);
    else {
	exeterm(env, nodevalue(SAVED2).lis);	/* split */
	env->dump2 = newnode2(env, env->stck, env->dump2);
	POP(env->stck);
	binrecaux(env);	/* first */
	GNULLARY(env->dump2);
	POP(env->dump2);
	binrecaux(env);	/* second */
	exeterm(env, nodevalue(SAVED1).lis);
    }	/* combine */
}

void binrec_(pEnv env)
{
    FOURPARAMS("binrec");
    FOURQUOTES("binrec");
    SAVESTACK;
    env->stck = SAVED5;
    binrecaux(env);
    POP(env->dump);
}
#endif
