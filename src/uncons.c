/*
    module  : uncons.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef UNCONS_C
#define UNCONS_C

/**
Q0  OK  2110  uncons  :  A  ->  F R
F and R are the first and the rest of non-empty aggregate A.
*/
void uncons_(pEnv env)
{
    int i = 0;
    char *str;
    uint64_t set;

    ONEPARAM("uncons");
    switch (nodetype(env->stck)) {
    case SET_:
	set = nodevalue(env->stck).set;
	CHECKEMPTYSET(set, "uncons");
	while (!(set & ((int64_t)1 << i)))
	    i++;
	UNARY(INTEGER_NEWNODE, i);
	NULLARY(SET_NEWNODE, set & ~((int64_t)1 << i));
	break;
    case STRING_:
	str = strdup((char *)&nodevalue(env->stck));
	CHECKEMPTYSTRING(str, "uncons");
	UNARY(CHAR_NEWNODE, *str);
	NULLARY(STRING_NEWNODE, str + 1);
	free(str);
	break;
    case LIST_:
	SAVESTACK;
	CHECKEMPTYLIST(nodevalue(SAVED1).lis, "uncons");
	GUNARY(nodevalue(SAVED1).lis);
	NULLARY(LIST_NEWNODE, nextnode1(nodevalue(SAVED1).lis));
	POP(env->dump);
	break;
    default:
	BADAGGREGATE("uncons");
    }
}
#endif
