/*
    module  : ufloat.h
    version : 1.2
    date    : 02/01/24
*/
#ifndef UFLOAT_H
#define UFLOAT_H

#define UFLOAT(PROCEDURE, NAME, FUNC)					\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	ONEPARAM(NAME);							\
	FLOAT(NAME);							\
	UNARY(FLOAT_NEWNODE, FUNC(FLOATVAL));				\
    }
#endif
