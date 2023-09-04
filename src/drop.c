/*
    module  : drop.c
    version : 1.7
    date    : 09/04/23
*/
#ifndef DROP_C
#define DROP_C

/**
OK 2130  drop  :  A N  ->  B
Aggregate B is the result of deleting the first N elements of A.
*/
PRIVATE void drop_(pEnv env)
{
    int n;

    TWOPARAMS("drop");
    POSITIVEINDEX(env->stck, "drop");
    n = nodevalue(env->stck).num;
    switch (nodetype(nextnode1(env->stck))) {
    case SET_: {
        int i;
        uint64_t result = 0;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(nextnode1(env->stck)).set & ((int64_t)1 << i)) {
                if (n < 1)
                    result |= ((int64_t)1 << i);
                else
                    n--;
            }
        BINARY(SET_NEWNODE, result);
        return;
    }
    case STRING_: {
        char *result = nodevalue(nextnode1(env->stck)).str;
        while (n-- > 0 && *result != '\0')
            ++result;
        BINARY(STRING_NEWNODE, GC_strdup(result));
        return;
    }
    case LIST_: {
        Index result = nodevalue(nextnode1(env->stck)).lis;
        while (n-- > 0 && result)
            result = nextnode1(result);
        BINARY(LIST_NEWNODE, result);
        return;
    }
    default:
        BADAGGREGATE("drop");
    }
}
#endif
