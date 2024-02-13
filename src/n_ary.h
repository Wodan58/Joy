/*
    module  : n_ary.h
    version : 1.3
    date    : 02/01/24
*/
#ifndef N_ARY_H
#define N_ARY_H

#define N_ARY(PROCEDURE, NAME, PARAMCOUNT, TOP)				\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	PARAMCOUNT(NAME);						\
	ONEQUOTE(NAME);							\
	SAVESTACK;							\
	POP(env->stck);							\
	exeterm(env, nodevalue(SAVED1).lis);				\
	CHECKVALUE(NAME);						\
	env->stck = newnode(env, nodetype(env->stck),			\
			    nodevalue(env->stck), TOP);			\
	POP(env->dump);							\
    }
#endif
