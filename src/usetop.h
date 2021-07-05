/*
    module  : usetop.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef USETOP_H
#define USETOP_H

#define USETOP(PROCEDURE, NAME, TYPE, BODY)                                    \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        TYPE(NAME);                                                            \
        BODY;                                                                  \
        POP(env->stck);                                                        \
    }
#endif
