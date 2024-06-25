/*
    module  : first.c
    version : 1.8
    date    : 06/21/24
*/
#ifndef FIRST_C
#define FIRST_C

/**
OK 2030  first  :  A  ->  F
F is the first member of the non-empty aggregate A.
*/
void first_(pEnv env)
{
    int i = 0;
    char *str;

    ONEPARAM("first");
    switch (nodetype(env->stck)) {
    case LIST_:
	CHECKEMPTYLIST(nodevalue(env->stck).lis, "first");
	GUNARY(nodevalue(env->stck).lis);
	break;
    case STRING_:
#ifdef NOBDW
	str = (char *)&nodevalue(env->stck);
#else
	str = nodevalue(env->stck).str;
#endif
	CHECKEMPTYSTRING(str, "first");
	UNARY(CHAR_NEWNODE, *str);
	break;
    case SET_:
	CHECKEMPTYSET(nodevalue(env->stck).set, "first");
	while (!(nodevalue(env->stck).set & ((int64_t)1 << i)))
	    i++;
	UNARY(INTEGER_NEWNODE, i);
	break;
    default:
	BADAGGREGATE("first");
    }
}
#endif
