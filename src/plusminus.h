/*
    module  : plusminus.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef PLUSMINUS_H
#define PLUSMINUS_H

#define PLUSMINUS(PROCEDURE, NAME, OPER)				\
    void PROCEDURE(pEnv env)						\
    {									\
	TWOPARAMS(NAME);						\
	FLOAT_I(OPER);							\
	INTEGER(NAME);							\
	NUMERIC2(NAME);							\
	if (nodetype(nextnode1(env->stck)) == CHAR_)			\
	    BINARY(CHAR_NEWNODE, nodevalue(nextnode1(env->stck)).num	\
		   OPER nodevalue(env->stck).num);			\
	else								\
	    BINARY(INTEGER_NEWNODE, nodevalue(nextnode1(env->stck)).num	\
		   OPER nodevalue(env->stck).num);			\
    }
#endif
