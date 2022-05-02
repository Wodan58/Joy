/*
    module  : _inhas.h
    version : 1.1
    date    : 05/02/22
*/
#ifndef _INHAS_H
#define _INHAS_H

#define _INHAS(PROCEDURE, NAME, AGGR, ELEM)                                    \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        int found = 0;                                                         \
        TWOPARAMS(NAME);                                                       \
        switch (nodetype(AGGR)) {                                              \
        case SET_:                                                             \
            CHECKSETMEMBER(ELEM, NAME);                                        \
            found                                                              \
                = ((nodevalue(AGGR).set) & ((long_t)1 << nodevalue(ELEM).num)) \
                > 0;                                                           \
            break;                                                             \
        case STRING_: {                                                        \
            char *s;                                                           \
            for (s = nodevalue(AGGR).str;                                      \
                 *s != '\0' && *s != nodevalue(ELEM).num; s++)                 \
                ;                                                              \
            found = *s != '\0';                                                \
            break;                                                             \
        }                                                                      \
        case LIST_: {                                                          \
            Index n = nodevalue(AGGR).lis;                                     \
	    while (n && nodevalue(n).num != nodevalue(ELEM).num)               \
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
