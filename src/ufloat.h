/*
    module  : ufloat.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef UFLOAT_H
#define UFLOAT_H

#define UFLOAT(PROCEDURE, NAME, FUNC)					\
    void PROCEDURE(pEnv env)						\
    {									\
	ONEPARAM(NAME);							\
	FLOAT(NAME);							\
	UNARY(FLOAT_NEWNODE, FUNC(FLOATVAL));				\
    }
#endif
