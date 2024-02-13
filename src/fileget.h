/*
    module  : fileget.h
    version : 1.2
    date    : 02/01/24
*/
#ifndef FILEGET_H
#define FILEGET_H

#define FILEGET(PROCEDURE, NAME, CONSTRUCTOR, EXPR)			\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	ONEPARAM(NAME);							\
	FILE(NAME);							\
	NULLARY(CONSTRUCTOR, EXPR);					\
    }
#endif
