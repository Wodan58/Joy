/*
    module  : inhas.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef INHAS_H
#define INHAS_H

#define INHAS(PROCEDURE, NAME, AGGR, ELEM)                                     \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        int found = 0, error;                                                  \
        TWOPARAMS(NAME);                                                       \
        switch (nodetype(AGGR)) {                                              \
        case SET_:                                                             \
            found                                                              \
                = ((nodevalue(AGGR).set) & ((long_t)1 << nodevalue(ELEM).num)) \
                > 0;                                                           \
            break;                                                             \
        case STRING_: {                                                        \
            char *s;                                                           \
            for (s = nodevalue(AGGR).str;                                      \
                 s && *s != '\0' && *s != nodevalue(ELEM).num; s++)            \
                ;                                                              \
            found = s && *s != '\0';                                           \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            Index n = nodevalue(AGGR).lis;                                     \
            while (n != NULL && (Compare(env, n, ELEM, &error) || error))      \
                n = nextnode1(n);                                              \
            found = n != NULL;                                                 \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
        BINARY(BOOLEAN_NEWNODE, found);                                        \
    }
#endif
