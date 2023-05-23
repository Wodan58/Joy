/*
    module  : comprel.h
    version : 1.5
    date    : 05/23/23
*/
#ifndef COMPREL_H
#define COMPREL_H

#define COMPREL(PROCEDURE, NAME, CONSTRUCTOR, OPR, SETCMP)                     \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        long i, j;                                                             \
        int comp = 0;                                                          \
        TWOPARAMS(NAME);                                                       \
        if (nodetype(env->stck) == SET_ ||                                     \
            nodetype(nextnode1(env->stck)) == SET_) {                          \
            i = nodevalue(nextnode1(env->stck)).num;                           \
            j = nodevalue(env->stck).num;                                      \
            comp = SETCMP;                                                     \
        } else                                                                 \
            comp = Compare(env, nextnode1(env->stck), env->stck) OPR 0;        \
        env->stck = CONSTRUCTOR(comp, nextnode2(env->stck));                   \
    }
#endif
