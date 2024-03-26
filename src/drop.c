/*
    module  : drop.c
    version : 1.9
    date    : 03/21/24
*/
#ifndef DROP_C
#define DROP_C

/**
OK 2130  drop  :  A N  ->  B
Aggregate B is the result of deleting the first N elements of A.
*/
void drop_(pEnv env)
{
    int i, n;
    char *str;
    Index list;
    uint64_t set;

    TWOPARAMS("drop");
    POSITIVEINDEX(env->stck, "drop");
    n = nodevalue(env->stck).num;
    switch (nodetype(nextnode1(env->stck))) {
    case SET_:
        for (set = i = 0; i < SETSIZE; i++)
            if (nodevalue(nextnode1(env->stck)).set & ((int64_t)1 << i)) {
                if (n < 1)
                    set |= ((int64_t)1 << i);
                else
                    n--;
            }
        BINARY(SET_NEWNODE, set);
        break;
    case STRING_:
        str = nodevalue(nextnode1(env->stck)).str;
        while (n-- > 0 && *str != '\0')
            ++str;
        BINARY(STRING_NEWNODE, GC_strdup(str));
        break;
    case LIST_:
        list = nodevalue(nextnode1(env->stck)).lis;
        while (n-- > 0 && list)
            list = nextnode1(list);
        BINARY(LIST_NEWNODE, list);
        break;
    default:
        BADAGGREGATE("drop");
    }
}
#endif
