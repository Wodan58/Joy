/*
    module  : ordchr.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef ORDCHR_H
#define ORDCHR_H

#define ORDCHR(PROCEDURE, NAME, RESULTTYP)                                     \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        NUMERICTYPE(NAME);                                                     \
        UNARY(RESULTTYP, nodevalue(env->stck).num);                            \
    }
#endif
