/*
    module  : ufloat.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef UFLOAT_H
#define UFLOAT_H

#define UFLOAT(PROCEDURE, NAME, FUNC)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        FLOAT(NAME);                                                           \
        UNARY(FLOAT_NEWNODE, FUNC(FLOATVAL));                                  \
    }
#endif
