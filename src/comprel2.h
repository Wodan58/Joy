/*
    module  : comprel2.h
    version : 1.1
    date    : 04/05/24
*/
#ifndef COMPREL2_H
#define COMPREL2_H

#define COMPREL2(PROCEDURE, NAME, CONSTRUCTOR, OPR)			\
    void PROCEDURE(pEnv env)						\
    {									\
	int comp;							\
	TWOPARAMS(NAME);						\
	comp = Compare(env, nextnode1(env->stck), env->stck) OPR 0;	\
	env->stck = CONSTRUCTOR(comp, nextnode2(env->stck));		\
    }
#endif
