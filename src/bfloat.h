/*
    module  : bfloat.h
    version : 1.3
    date    : 03/21/24
*/
#ifndef BFLOAT_H
#define BFLOAT_H

#define BFLOAT(PROCEDURE, NAME, FUNC)					\
    void PROCEDURE(pEnv env)						\
    {									\
	TWOPARAMS(NAME);						\
	FLOAT2(NAME);							\
	BINARY(FLOAT_NEWNODE, FUNC(FLOATVAL2, FLOATVAL));		\
    }
#endif
