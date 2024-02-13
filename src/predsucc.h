/*
    module  : predsucc.h
    version : 1.2
    date    : 02/01/24
*/
#ifndef PREDSUCC_H
#define PREDSUCC_H

#define PREDSUCC(PROCEDURE, NAME, OPER)					\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	ONEPARAM(NAME);							\
	NUMERICTYPE(NAME);						\
	if (nodetype(env->stck) == CHAR_)				\
	    UNARY(CHAR_NEWNODE, nodevalue(env->stck).num OPER 1);	\
	else								\
	    UNARY(INTEGER_NEWNODE, nodevalue(env->stck).num OPER 1);	\
    }
#endif
