/*
    module  : fileget.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef FILEGET_H
#define FILEGET_H

#define FILEGET(PROCEDURE, NAME, CONSTRUCTOR, EXPR)			\
    void PROCEDURE(pEnv env)						\
    {									\
	ONEPARAM(NAME);							\
	FILE(NAME);							\
	NULLARY(CONSTRUCTOR, EXPR);					\
    }
#endif
