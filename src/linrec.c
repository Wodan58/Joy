/*
    module  : linrec.c
    version : 1.10
    date    : 11/11/24
*/
#ifndef LINREC_C
#define LINREC_C

#include "boolean.h"

/**
Q4  OK  2710  linrec  :  [P] [T] [R1] [R2]  ->  ...
Executes P. If that yields true, executes T.
Else executes R1, recurses, executes R2.
*/
void linrecaux(pEnv env)
{
    int result;

    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED4).lis);
    CHECKSTACK("linrec");
    result = get_boolean(env, env->stck);
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
	exeterm(env, nodevalue(SAVED3).lis);
    else {
	exeterm(env, nodevalue(SAVED2).lis);
	linrecaux(env);
	exeterm(env, nodevalue(SAVED1).lis);
    }
}

void linrec_(pEnv env)
{
    FOURPARAMS("linrec");
    FOURQUOTES("linrec");
    SAVESTACK;
    env->stck = SAVED5;
    linrecaux(env);
    POP(env->dump);
}
#endif
