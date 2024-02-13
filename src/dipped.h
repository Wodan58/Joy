/*
    module  : dipped.h
    version : 1.2
    date    : 02/01/24
*/
#ifndef DIPPED_H
#define DIPPED_H

#define DIPPED(PROCEDURE, NAME, PARAMCOUNT, ARGUMENT)			\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	PARAMCOUNT(NAME);						\
	SAVESTACK;							\
	POP(env->stck);							\
	ARGUMENT(env);							\
	GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));			\
	POP(env->dump);							\
    }
#endif
