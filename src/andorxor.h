/*
    module  : andorxor.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef ANDORXOR_H
#define ANDORXOR_H

#define ANDORXOR(PROCEDURE, NAME, OPER1, OPER2)				\
    void PROCEDURE(pEnv env)						\
    {									\
	TWOPARAMS(NAME);						\
	SAME2TYPES(NAME);						\
	switch (nodetype(env->stck)) {					\
	case SET_:							\
	    BINARY(SET_NEWNODE, nodevalue(nextnode1(env->stck)).set	\
		   OPER1 nodevalue(env->stck).set);			\
	    return;							\
	case BOOLEAN_:							\
	case CHAR_:							\
	case INTEGER_:							\
	    BINARY(BOOLEAN_NEWNODE, nodevalue(nextnode1(env->stck)).num	\
		   OPER2 nodevalue(env->stck).num);			\
	    return;							\
	default:							\
	    BADDATA(NAME);						\
	}								\
    }
#endif
