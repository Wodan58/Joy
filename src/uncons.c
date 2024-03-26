/*
    module  : uncons.c
    version : 1.7
    date    : 03/21/24
*/
#ifndef UNCONS_C
#define UNCONS_C

/**
OK 2110  uncons  :  A  ->  F R
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
        str = nodevalue(env->stck).str;
        CHECKEMPTYSTRING(str, "uncons");
        UNARY(CHAR_NEWNODE, *str);
        NULLARY(STRING_NEWNODE, GC_strdup(++str));
        break;
    case LIST_:
        SAVESTACK;
        CHECKEMPTYLIST(nodevalue(SAVED1).lis, "uncons");
        GUNARY(
            nodetype(nodevalue(SAVED1).lis), nodevalue(nodevalue(SAVED1).lis));
        NULLARY(LIST_NEWNODE, nextnode1(nodevalue(SAVED1).lis));
        POP(env->dump);
        break;
    default:
        BADAGGREGATE("uncons");
    }
}
#endif
