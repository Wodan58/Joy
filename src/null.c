/*
    module  : null.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef NULL_C
#define NULL_C

/**
Q0  OK  2200  null  :  X  ->  B
Tests for empty aggregate X or zero numeric.
*/
void null_(pEnv env)
{
    ONEPARAM("null");
    switch (nodetype(env->stck)) {
#if 0
    case USR_:
	UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).ent));
	break;
    case ANON_FUNCT_:
	UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).proc));
	break;
#endif
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
	UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).num));
	break;
    case SET_:
	UNARY(BOOLEAN_NEWNODE, (!nodevalue(env->stck).set));
	break;
    case STRING_:
#ifdef NOBDW
	UNARY(BOOLEAN_NEWNODE, (!nodeleng(env->stck)));
#else
	UNARY(BOOLEAN_NEWNODE, (!*(nodevalue(env->stck).str)));
#endif
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
    default:
	UNARY(BOOLEAN_NEWNODE, 0);	/* false */
	break;
    }
}
#endif
