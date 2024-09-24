/*
    module  : fileget.h
    version : 1.4
    date    : 09/17/24
*/
#ifndef FILEGET_H
#define FILEGET_H

#define FILEGET(PROCEDURE, NAME, CONSTRUCTOR, EXPR)			\
    void PROCEDURE(pEnv env)						\
    {									\
	ONEPARAM(NAME);							\
	ISFILE(NAME);							\
	NULLARY(CONSTRUCTOR, EXPR);					\
    }
#endif
