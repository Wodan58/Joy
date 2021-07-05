/*
    module  : not.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef NOT_C
#define NOT_C

/**
1370  not  :  X  ->  Y
Y is the complement of set X, logical negation for truth values.
*/
PRIVATE void not_(pEnv env)
{
    ONEPARAM("not");
    switch (nodetype(env->stck)) {
    case SET_:
        UNARY(SET_NEWNODE, ~nodevalue(env->stck).set);
        break;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        UNARY(BOOLEAN_NEWNODE, !nodevalue(env->stck).num);
        break;
    default:
        BADDATA("not");
    }
}
#endif
