/*
    module  : first.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FIRST_C
#define FIRST_C

/**
2040  first  :  A  ->  F
F is the first member of the non-empty aggregate A.
*/
PRIVATE void first_(pEnv env)
{
    ONEPARAM("first");
    switch (nodetype(env->stck)) {
    case LIST_:
        CHECKEMPTYLIST(nodevalue(env->stck).lis, "first");
        GUNARY(nodetype(nodevalue(env->stck).lis),
            nodevalue(nodevalue(env->stck).lis));
        return;
    case STRING_:
        CHECKEMPTYSTRING(nodevalue(env->stck).str, "first");
        UNARY(CHAR_NEWNODE, *(nodevalue(env->stck).str));
        return;
    case SET_: {
        int i = 0;
        CHECKEMPTYSET(nodevalue(env->stck).set, "first");
        while (!(nodevalue(env->stck).set & ((long_t)1 << i)))
            i++;
        UNARY(INTEGER_NEWNODE, i);
        return;
    }
    default:
        BADAGGREGATE("first");
    }
}
#endif
