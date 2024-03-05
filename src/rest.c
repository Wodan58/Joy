/*
    module  : rest.c
    version : 1.6
    date    : 03/05/24
*/
#ifndef REST_C
#define REST_C

/**
OK 2040  rest  :  A  ->  R
R is the non-empty aggregate A with its first member removed.
*/
PRIVATE void rest_(pEnv env)
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
        str = nodevalue(env->stck).str;
        CHECKEMPTYSTRING(str, "rest");
        UNARY(STRING_NEWNODE, GC_strdup(++str));
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
