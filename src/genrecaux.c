/*
    module  : genrecaux.c
    version : 1.10
    date    : 11/11/24
*/
#ifndef GENRECAUX_C
#define GENRECAUX_C

#include "boolean.h"

/**
Q1  OK  3240  #genrec  :  [[B] [T] [R1] R2]  ->  ...
Executes B, if that yields true, executes T.
Else executes R1 and then [[[B] [T] [R1] R2] genrec] R2.
*/
void genrecaux_(pEnv env)
{
    Index temp;
    int result;

    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(nodevalue(SAVED1).lis).lis);		/* [B] */
    CHECKSTACK("genrecaux");
    result = get_boolean(env, env->stck);
    env->stck = SAVED2;
    if (result)
	exeterm(env, nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis); /* [T] */
    else {
	exeterm(env, nodevalue(nextnode2(nodevalue(SAVED1).lis)).lis);/* [R1] */
	NULLARY(LIST_NEWNODE, nodevalue(SAVED1).lis);
	temp = ANON_FUNCT_NEWNODE(genrecaux_, 0);
	NULLARY(LIST_NEWNODE, temp);
	cons_(env);
	exeterm(env, nextnode3(nodevalue(SAVED1).lis));		/* [R2] */
    }
    POP(env->dump);
}
#endif
