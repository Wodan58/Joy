/*
    module  : someall.h
    version : 1.2
    date    : 05/02/22
*/
#ifndef SOMEALL_H
#define SOMEALL_H

#define SOMEALL(PROCEDURE, NAME, INITIAL)                                      \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        int result = INITIAL;                                                  \
        TWOPARAMS(NAME);                                                       \
        ONEQUOTE(NAME);                                                        \
        SAVESTACK;                                                             \
        switch (nodetype(SAVED2)) {                                            \
        case SET_: {                                                           \
            int j;                                                             \
            for (j = 0; j < SETSIZE && result == INITIAL; j++) {               \
                if (nodevalue(SAVED2).set & ((long_t)1 << j)) {                \
                    env->stck = INTEGER_NEWNODE(j, SAVED3);                    \
                    exeterm(env, nodevalue(SAVED1).lis);                       \
                    CHECKSTACK(NAME);                                          \
                    if (nodevalue(env->stck).num != INITIAL)                   \
                        result = 1 - INITIAL;                                  \
                }                                                              \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        case STRING_: {                                                        \
            char *s;                                                           \
            for (s = nodevalue(SAVED2).str; *s != '\0' && result == INITIAL;   \
                 s++) {                                                        \
                env->stck = CHAR_NEWNODE(*s, SAVED3);                          \
                exeterm(env, nodevalue(SAVED1).lis);                           \
                CHECKSTACK(NAME);                                              \
                if (nodevalue(env->stck).num != INITIAL)                       \
                    result = 1 - INITIAL;                                      \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            env->dump1 = newnode(env, LIST_, nodevalue(SAVED2), env->dump1);   \
            while (DMP1 != NULL && result == INITIAL) {                        \
                env->stck                                                      \
                    = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);   \
                exeterm(env, nodevalue(SAVED1).lis);                           \
                CHECKSTACK(NAME);                                              \
                if (nodevalue(env->stck).num != INITIAL)                       \
                    result = 1 - INITIAL;                                      \
                DMP1 = nextnode1(DMP1);                                        \
            }                                                                  \
            POP(env->dump1);                                                   \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        env->stck = BOOLEAN_NEWNODE(result, SAVED3);                           \
        POP(env->dump);                                                        \
    }
#endif
