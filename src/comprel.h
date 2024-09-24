/*
    module  : comprel.h
    version : 1.9
    date    : 09/17/24
*/
#ifndef COMPREL_H
#define COMPREL_H

#define COMPREL(PROCEDURE, NAME, CONSTRUCTOR, OPR, SETCMP)		\
    void PROCEDURE(pEnv env)						\
    {									\
	uint64_t i, j;							\
	int comp = 0;							\
	TWOPARAMS(NAME);						\
	if (nodetype(env->stck) == SET_ ||				\
	    nodetype(nextnode1(env->stck)) == SET_) {			\
	    i = nodevalue(nextnode1(env->stck)).num;			\
	    j = nodevalue(env->stck).num;				\
	    comp = SETCMP;						\
	} else								\
	    comp = Compare(env, nextnode1(env->stck), env->stck) OPR 0;	\
	env->stck = CONSTRUCTOR(comp, nextnode2(env->stck));		\
    }
#endif
