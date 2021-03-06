/*
    module  : comprel.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef COMPREL_H
#define COMPREL_H

#define COMPREL(PROCEDURE, NAME, CONSTRUCTOR, OPR, SETCMP)                     \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        double cmp;                                                            \
        int comp = 0, error, i, j;                                             \
        TWOPARAMS(NAME);                                                       \
        if (nodetype(env->stck) == SET_) {                                     \
            i = nodevalue(nextnode1(env->stck)).num;                           \
            j = nodevalue(env->stck).num;                                      \
            comp = SETCMP;                                                     \
        } else {                                                               \
            cmp = Compare(env, nextnode1(env->stck), env->stck, &error);       \
            if (error)                                                         \
                BADDATA(NAME);                                                 \
            else {                                                             \
                comp = (int)(cmp OPR 0);                                       \
                if (comp < 0)                                                  \
                    comp = -1;                                                 \
                else if (comp > 0)                                             \
                    comp = 1;                                                  \
            }                                                                  \
        }                                                                      \
        env->stck = CONSTRUCTOR(comp, nextnode2(env->stck));                   \
    }
#endif
