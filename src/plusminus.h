/*
    module  : plusminus.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef PLUSMINUS_H
#define PLUSMINUS_H

#define PLUSMINUS(PROCEDURE, NAME, OPER)                                       \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        FLOAT_I(OPER);                                                         \
        INTEGER(NAME);                                                         \
        NUMERIC2(NAME);                                                        \
        if (nodetype(nextnode1(env->stck)) == CHAR_)                           \
            BINARY(CHAR_NEWNODE, nodevalue(nextnode1(env->stck))               \
                                     .num OPER nodevalue(env->stck)            \
                                     .num);                                    \
        else                                                                   \
            BINARY(INTEGER_NEWNODE, nodevalue(nextnode1(env->stck))            \
                                        .num OPER nodevalue(env->stck)         \
                                        .num);                                 \
    }
#endif
