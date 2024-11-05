/*
    module  : rest.c
    version : 1.13
    date    : 10/28/24
*/
#ifndef REST_C
#define REST_C

/**
Q0  OK  2040  rest  :  A  ->  R
R is the non-empty aggregate A with its first member removed.
*/
void rest_(pEnv env)
{
    int i = 0;
    char *str;

    ONEPARAM("rest");
    switch (nodetype(env->stck)) {
    case SET_:
	CHECKEMPTYSET(nodevalue(env->stck).set, "rest");
	while (!(nodevalue(env->stck).set & ((int64_t)1 << i)))
	    i++;
	UNARY(SET_NEWNODE, nodevalue(env->stck).set & ~((int64_t)1 << i));
	break;
    case STRING_:
	str = GETSTRING(env->stck);
	CHECKEMPTYSTRING(str, "rest");
#ifdef NOBDW
	str = strdup(str + 1);
	UNARY(STRING_NEWNODE, str);
	free(str);
#else
	UNARY(STRING_NEWNODE, GC_strdup(str + 1));
#endif
	break;
    case LIST_:
	CHECKEMPTYLIST(nodevalue(env->stck).lis, "rest");
	UNARY(LIST_NEWNODE, nextnode1(nodevalue(env->stck).lis));
	break;
    default:
	BADAGGREGATE("rest");
    }
}
#endif
