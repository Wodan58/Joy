/*
    module  : ordchr.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef ORDCHR_H
#define ORDCHR_H

#define ORDCHR(PROCEDURE, NAME, RESULTTYP)				\
    void PROCEDURE(pEnv env)						\
    {									\
	ONEPARAM(NAME);							\
	NUMERICTYPE(NAME);						\
	UNARY(RESULTTYP, nodevalue(env->stck).num);			\
    }
#endif
