/*
    module  : take.c
    version : 1.9
    date    : 03/21/24
*/
#ifndef TAKE_C
#define TAKE_C

/**
OK 2140  take  :  A N  ->  B
Aggregate B is the result of retaining just the first N elements of A.
*/
void take_(pEnv env)
{
    int i, n;
    uint64_t result;
    char *old, *ptr, *resultstr;

    TWOPARAMS("take");
    POSITIVEINDEX(env->stck, "take");
    n = nodevalue(env->stck).num;
    switch (nodetype(nextnode1(env->stck))) {
    case SET_:
        for (result = i = 0; i < SETSIZE; i++)
            if (nodevalue(nextnode1(env->stck)).set & ((int64_t)1 << i)) {
                if (n > 0) {
                    --n;
                    result |= ((int64_t)1 << i);
                } else
                    break;
            }
        BINARY(SET_NEWNODE, result);
        break;
    case STRING_:
        old = nodevalue(nextnode1(env->stck)).str;
        POP(env->stck);
        /* do not swap the order of the next two statements ! ! ! */
#if 0
        if (n < 0)
            n = 0;
#endif
        if (n >= (int)strlen(old))
            return; /* the old string unchanged */
        ptr = resultstr = GC_malloc_atomic(n + 1);
        while (n-- > 0)
            *ptr++ = *old++;
        *ptr = 0;
        UNARY(STRING_NEWNODE, resultstr);
        break;
    case LIST_:
#if 0
        if (n < 1) {
            BINARY(LIST_NEWNODE, 0);
            return;
        } /* null string */
#endif
        env->dump1 = newnode(
            env, LIST_, nodevalue(nextnode1(env->stck)), env->dump1); /* old  */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last */
        while (DMP1 && n-- > 0) {
            if (!DMP2) { /* first */
                DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                DMP3 = DMP2;
            } else { /* further */
                nextnode1(DMP3)
                    = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                DMP3 = nextnode1(DMP3);
            }
            DMP1 = nextnode1(DMP1);
        }
        nextnode1(DMP3) = 0;
        BINARY(LIST_NEWNODE, DMP2);
        POP(env->dump1);
        POP(env->dump2);
        POP(env->dump3);
        break;
    default:
        BADAGGREGATE("take");
    }
}
#endif
