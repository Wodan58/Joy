/*
    module  : cons_swons.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef CONS_SWONS_H
#define CONS_SWONS_H

#define CONS_SWONS(PROCEDURE, NAME, AGGR, ELEM)                                \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        TWOPARAMS(NAME);                                                       \
        switch (nodetype(AGGR)) {                                              \
        case LIST_:                                                            \
            BINARY(LIST_NEWNODE, newnode(env, nodetype(ELEM), nodevalue(ELEM), \
                                     nodevalue(AGGR).lis));                    \
            break;                                                             \
        case SET_:                                                             \
            CHECKSETMEMBER(ELEM, NAME);                                        \
            BINARY(SET_NEWNODE,                                                \
                nodevalue(AGGR).set | ((long_t)1 << nodevalue(ELEM).num));     \
            break;                                                             \
        case STRING_: {                                                        \
            char *s;                                                           \
            if (nodetype(ELEM) != CHAR_)                                       \
                execerror(env, "character", NAME);                             \
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
