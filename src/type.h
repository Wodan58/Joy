/*
    module  : type.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef TYPE_H
#define TYPE_H

#define TYPE(PROCEDURE, NAME, REL, TYP)					\
    void PROCEDURE(pEnv env)						\
    {									\
	ONEPARAM(NAME);							\
	UNARY(BOOLEAN_NEWNODE, (nodetype(env->stck) REL TYP));		\
    }
#endif
