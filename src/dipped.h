/*
    module  : dipped.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef DIPPED_H
#define DIPPED_H

#define DIPPED(PROCEDURE, NAME, PARAMCOUNT, ARGUMENT)			\
    void PROCEDURE(pEnv env)						\
    {									\
	PARAMCOUNT(NAME);						\
	SAVESTACK;							\
	POP(env->stck);							\
	ARGUMENT(env);							\
	GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));			\
	POP(env->dump);							\
    }
#endif
