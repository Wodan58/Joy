/*
    module  : bfloat.h
    version : 1.2
    date    : 02/01/24
*/
#ifndef BFLOAT_H
#define BFLOAT_H

#define BFLOAT(PROCEDURE, NAME, FUNC)					\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	TWOPARAMS(NAME);						\
	FLOAT2(NAME);							\
	BINARY(FLOAT_NEWNODE, FUNC(FLOATVAL2, FLOATVAL));		\
    }
#endif
