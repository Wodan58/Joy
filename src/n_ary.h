/*
    module  : n_ary.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef N_ARY_H
#define N_ARY_H

#define N_ARY(PROCEDURE, NAME, PARAMCOUNT, TOP)                                \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        PARAMCOUNT(NAME);                                                      \
        ONEQUOTE(NAME);                                                        \
        SAVESTACK;                                                             \
        POP(env->stck);                                                        \
        exeterm(env, nodevalue(SAVED1).lis);                                   \
        if (env->stck == NULL)                                                 \
            execerror(env, "value to push", NAME);                             \
        env->stck                                                              \
            = newnode(env, nodetype(env->stck), nodevalue(env->stck), TOP);    \
        POP(env->dump);                                                        \
    }
#endif
