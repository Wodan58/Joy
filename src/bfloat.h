/*
    module  : bfloat.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef BFLOAT_H
#define BFLOAT_H

#define BFLOAT(PROCEDURE, NAME, FUNC)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        FLOAT2(NAME);                                                          \
        BINARY(FLOAT_NEWNODE, FUNC(FLOATVAL2, FLOATVAL));                      \
    }
#endif
