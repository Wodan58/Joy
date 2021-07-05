/*
    module  : fileget.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef FILEGET_H
#define FILEGET_H

#define FILEGET(PROCEDURE, NAME, CONSTRUCTOR, EXPR)                            \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        FILE(NAME);                                                            \
        NULLARY(CONSTRUCTOR, EXPR);                                            \
    }
#endif
