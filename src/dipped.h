/*
    module  : dipped.h
    version : 1.4
    date    : 06/21/24
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
	GNULLARY(SAVED1);						\
	POP(env->dump);							\
    }
#endif
