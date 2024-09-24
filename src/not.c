/*
    module  : not.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef NOT_C
#define NOT_C

/**
Q0  OK  1370  not  :  X  ->  Y
Y is the complement of set X, logical negation for truth values.
*/
void not_(pEnv env)
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
