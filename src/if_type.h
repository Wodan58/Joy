/*
    module  : if_type.h
    version : 1.2
    date    : 05/02/22
*/
#ifndef IF_TYPE_H
#define IF_TYPE_H

#define IF_TYPE(PROCEDURE, NAME, TYP)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        THREEPARAMS(NAME);                                                     \
        TWOQUOTES(NAME);                                                       \
        SAVESTACK;                                                             \
        env->stck = SAVED3;                                                    \
        exeterm(env, nodetype(env->stck) == TYP ? nodevalue(SAVED2).lis        \
                                                : nodevalue(SAVED1).lis);      \
        POP(env->dump);                                                        \
    }
#endif
