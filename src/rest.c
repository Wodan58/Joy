/*
    module  : rest.c
    version : 1.8
    date    : 06/21/24
*/
#ifndef REST_C
#define REST_C

/**
OK 2040  rest  :  A  ->  R
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
#ifdef NOBDW
	str = strdup((char *)&nodevalue(env->stck));
#else
	str = nodevalue(env->stck).str;
#endif
	CHECKEMPTYSTRING(str, "rest");
#ifdef NOBDW
	UNARY(STRING_NEWNODE, str + 1);
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
