/*
    module  : cons_swons.h
    version : 1.6
    date    : 07/19/23
*/
#ifndef CONS_SWONS_H
#define CONS_SWONS_H

#define CONS_SWONS(PROCEDURE, NAME, AGGR, ELEM)                                \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        Index temp;                                                            \
        TWOPARAMS(NAME);                                                       \
        switch (nodetype(AGGR)) {                                              \
        case LIST_:                                                            \
            temp = newnode(env, nodetype(ELEM), nodevalue(ELEM),               \
                                nodevalue(AGGR).lis);                          \
            BINARY(LIST_NEWNODE, temp);                                        \
            break;                                                             \
        case SET_:                                                             \
            CHECKSETMEMBER(ELEM, NAME);                                        \
            BINARY(SET_NEWNODE,                                                \
                nodevalue(AGGR).set | ((int64_t)1 << nodevalue(ELEM).num));    \
            break;                                                             \
        case STRING_: {                                                        \
            char *s;                                                           \
            CHECKCHARACTER(ELEM, NAME);                                        \
            s = (char *)GC_malloc_atomic(strlen(nodevalue(AGGR).str) + 2);     \
            s[0] = (char)nodevalue(ELEM).num;                                  \
            strcpy(s + 1, nodevalue(AGGR).str);                                \
            BINARY(STRING_NEWNODE, s);                                         \
            break;                                                             \
        }                                                                      \
        default:                                                               \
            BADAGGREGATE(NAME);                                                \
        }                                                                      \
    }
#endif
