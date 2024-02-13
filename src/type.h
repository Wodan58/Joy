/*
    module  : type.h
    version : 1.2
    date    : 02/01/24
*/
#ifndef TYPE_H
#define TYPE_H

#define TYPE(PROCEDURE, NAME, REL, TYP)					\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	ONEPARAM(NAME);							\
	UNARY(BOOLEAN_NEWNODE, (nodetype(env->stck) REL TYP));		\
    }
#endif
