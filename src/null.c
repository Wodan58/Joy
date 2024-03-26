/*
    module  : null.c
    version : 1.6
    date    : 03/21/24
*/
#ifndef NULL_C
#define NULL_C

/**
OK 2200  null  :  X  ->  B
Tests for empty aggregate X or zero numeric.
*/
void null_(pEnv env)
{
    ONEPARAM("null");
    switch (nodetype(env->stck)) {
    case USR_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).ent));
        break;
    case ANON_FUNCT_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).proc));
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).num));
        break;
    case SET_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).set));
        break;
    case STRING_:
        UNARY(BOOLEAN_NEWNODE, (!*(nodevalue(env->stck).str)));
        break;
    case LIST_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).lis));
        break;
    case FLOAT_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).dbl));
        break;
    case FILE_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).fil));
        break;
    }
}
#endif
