/*
    module  : if_type.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef IF_TYPE_H
#define IF_TYPE_H

#define IF_TYPE(PROCEDURE, NAME, TYP)                                          \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        TWOQUOTES(NAME);                                                       \
        SAVESTACK;                                                             \
        env->stck = SAVED3;                                                    \
        exeterm(env, nodetype(env->stck) == TYP ? nodevalue(SAVED2).lis        \
                                                : nodevalue(SAVED1).lis);      \
        POP(env->dump);                                                        \
    }
#endif
