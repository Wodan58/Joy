/*
    module  : type.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef TYPE_H
#define TYPE_H

#define TYPE(PROCEDURE, NAME, REL, TYP)                                        \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        ONEPARAM(NAME);                                                        \
        UNARY(BOOLEAN_NEWNODE, (nodetype(env->stck) REL TYP));                 \
    }
#endif
