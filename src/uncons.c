/*
    module  : uncons.c
    version : 1.2
    date    : 05/17/22
*/
#ifndef UNCONS_C
#define UNCONS_C

/**
2120  uncons  :  A  ->  F R
F and R are the first and the rest of non-empty aggregate A.
*/
PRIVATE void uncons_(pEnv env)
{
    ONEPARAM("uncons");
    switch (nodetype(env->stck)) {
    case SET_: {
        int i = 0;
        long set = nodevalue(env->stck).set;
        CHECKEMPTYSET(set, "uncons");
        while (!(set & ((long)1 << i)))
            i++;
        UNARY(INTEGER_NEWNODE, i);
        NULLARY(SET_NEWNODE, set & ~((long)1 << i));
        break;
    }
    case STRING_: {
        char *s = nodevalue(env->stck).str;
        CHECKEMPTYSTRING(s, "uncons");
        UNARY(CHAR_NEWNODE, *s);
        NULLARY(STRING_NEWNODE, GC_strdup(++s));
        break;
    }
    case LIST_:
        SAVESTACK;
        CHECKEMPTYLIST(nodevalue(SAVED1).lis, "uncons");
        GUNARY(
            nodetype(nodevalue(SAVED1).lis), nodevalue(nodevalue(SAVED1).lis));
        NULLARY(LIST_NEWNODE, nextnode1(nodevalue(SAVED1).lis));
        POP(env->dump);
        return;
    default:
        BADAGGREGATE("uncons");
    }
}
#endif
