/*
    module  : null.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef NULL_C
#define NULL_C

/**
2210  null  :  X  ->  B
Tests for empty aggregate X or zero numeric.
*/
PRIVATE void null_(pEnv env)
{
    ONEPARAM("null");
    switch (nodetype(env->stck)) {
    case STRING_:
        UNARY(BOOLEAN_NEWNODE, (!*(nodevalue(env->stck).str)));
        break;
    case FLOAT_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).dbl));
        break;
    case FILE_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).fil));
        break;
    case LIST_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).lis));
        break;
    case SET_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).set));
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).num));
        break;
    default:
        BADDATA("null");
    }
}
#endif
