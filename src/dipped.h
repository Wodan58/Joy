/*
    module  : dipped.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef DIPPED_H
#define DIPPED_H

#define DIPPED(PROCEDURE, NAME, PARAMCOUNT, ARGUMENT)                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        PARAMCOUNT(NAME);                                                      \
        SAVESTACK;                                                             \
        POP(env->stck);                                                        \
        ARGUMENT(env);                                                         \
        GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));                         \
        POP(env->dump);                                                        \
    }
#endif
