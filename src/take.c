/*
    module  : take.c
    version : 1.2
    date    : 05/02/22
*/
#ifndef TAKE_C
#define TAKE_C

/**
2150  take  :  A N  ->  B
Aggregate B is the result of retaining just the first N elements of A.
*/
PRIVATE void take_(pEnv env)
{
    int n;

    TWOPARAMS("take");
    POSITIVEINDEX(env->stck, "take");
    n = nodevalue(env->stck).num;
    switch (nodetype(nextnode1(env->stck))) {
    case SET_: {
        int i;
        long_t result = 0;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(nextnode1(env->stck)).set & ((long_t)1 << i)) {
                if (n > 0) {
                    --n;
                    result |= ((long_t)1 << i);
                } else
                    break;
            }
        BINARY(SET_NEWNODE, result);
        return;
    }
    case STRING_: {
        int i;
        char *old, *p, *result;
        i = nodevalue(env->stck).num;
        old = nodevalue(nextnode1(env->stck)).str;
        POP(env->stck);
        /* do not swap the order of the next two statements ! ! ! */
#if 0
        if (i < 0)
            i = 0;
#endif
        if ((size_t)i >= strlen(old))
            return; /* the old string unchanged */
        p = result = (char *)GC_malloc_atomic(i + 1);
        while (i-- > 0)
            *p++ = *old++;
        *p = 0;
        UNARY(STRING_NEWNODE, result);
        return;
    }
    case LIST_: {
        int i = nodevalue(env->stck).num;
        if (i < 1) {
            BINARY(LIST_NEWNODE, NULL);
            return;
        } /* null string */
        env->dump1 = newnode(
            env, LIST_, nodevalue(nextnode1(env->stck)), env->dump1); /* old  */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last */
        while (DMP1 != NULL && i-- > 0) {
            if (DMP2 == NULL) /* first */
            {
                DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP3 = DMP2;
            } else /* further */
            {
                nextnode1(DMP3)
                    = NEWNODE(nodetype(DMP1), nodevalue(DMP1), NULL);
                DMP3 = nextnode1(DMP3);
            }
            DMP1 = nextnode1(DMP1);
        }
        nextnode1(DMP3) = NULL;
        BINARY(LIST_NEWNODE, DMP2);
        POP(env->dump1);
        POP(env->dump2);
        POP(env->dump3);
        return;
    }
    default:
        BADAGGREGATE("take");
    }
}
#endif
