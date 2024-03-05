/*
    module  : first.c
    version : 1.6
    date    : 03/05/24
*/
#ifndef FIRST_C
#define FIRST_C

/**
OK 2030  first  :  A  ->  F
F is the first member of the non-empty aggregate A.
*/
PRIVATE void first_(pEnv env)
{
    int i = 0;

    ONEPARAM("first");
    switch (nodetype(env->stck)) {
    case LIST_:
        CHECKEMPTYLIST(nodevalue(env->stck).lis, "first");
        GUNARY(nodetype(nodevalue(env->stck).lis),
            nodevalue(nodevalue(env->stck).lis));
        break;
    case STRING_:
        CHECKEMPTYSTRING(nodevalue(env->stck).str, "first");
        UNARY(CHAR_NEWNODE, *(nodevalue(env->stck).str));
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
