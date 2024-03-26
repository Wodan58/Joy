/*
    module  : maxmin.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef MAXMIN_H
#define MAXMIN_H

#define MAXMIN(PROCEDURE, NAME, OPER)					\
    void PROCEDURE(pEnv env)						\
    {									\
	TWOPARAMS(NAME);						\
	if (FLOATABLE2) {						\
	    BINARY(FLOAT_NEWNODE, FLOATVAL OPER FLOATVAL2 ?		\
		   FLOATVAL2 : FLOATVAL);				\
	    return;							\
	}								\
	SAME2TYPES(NAME);						\
	NUMERICTYPE(NAME);						\
	if (nodetype(env->stck) == CHAR_)				\
	    BINARY(CHAR_NEWNODE, nodevalue(env->stck).num OPER		\
		   nodevalue(nextnode1(env->stck)).num ?		\
		   nodevalue(nextnode1(env->stck)).num :		\
		   nodevalue(env->stck).num);				\
	else								\
	    BINARY(INTEGER_NEWNODE, nodevalue(env->stck).num OPER	\
		   nodevalue(nextnode1(env->stck)).num ?		\
		   nodevalue(nextnode1(env->stck)).num :		\
		   nodevalue(env->stck).num);				\
    }
#endif
