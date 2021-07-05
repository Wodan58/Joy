/*
    module  : filter.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FILTER_C
#define FILTER_C

/**
2850  filter  :  A [B]  ->  A1
Uses test B to filter aggregate A producing sametype aggregate A1.
*/
PRIVATE void filter_(pEnv env)
{
    TWOPARAMS("filter");
    ONEQUOTE("filter");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_: {
        int j;
        long_t resultset = 0;
        for (j = 0; j < SETSIZE; j++) {
            if (nodevalue(SAVED2).set & ((long_t)1 << j)) {
                env->stck = INTEGER_NEWNODE(j, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                if (nodevalue(env->stck).num)
                    resultset |= ((long_t)1 << j);
            }
        }
        env->stck = SET_NEWNODE(resultset, SAVED3);
        break;
    }
    case STRING_: {
        char *s, *resultstring;
        int j = 0;
        resultstring
            = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long_t)*s, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (nodevalue(env->stck).num)
                resultstring[j++] = *s;
        }
        resultstring[j] = '\0';
        env->stck = STRING_NEWNODE(resultstring, SAVED3);
        break;
    }
    case LIST_: {
        env->dump1
            = newnode(env, LIST_, nodevalue(SAVED2), env->dump1); /* step old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head new */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last new */
        while (DMP1 != NULL) {
            env->stck = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            if (nodevalue(env->stck).num) /* test */
            {
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
            }
            DMP1 = nextnode1(DMP1);
        }
        env->stck = LIST_NEWNODE(DMP2, SAVED3);
        POP(env->dump3);
        POP(env->dump2);
        POP(env->dump1);
        break;
    }
    default:
        BADAGGREGATE("filter");
    }
    POP(env->dump);
}
#endif
