/*
    module  : map.c
    version : 1.7
    date    : 03/05/24
*/
#ifndef MAP_C
#define MAP_C

/**
OK 2790  map  :  A [P]  ->  B
Executes P on each member of aggregate A,
collects results in sametype aggregate B.
*/
PRIVATE void map_(pEnv env)
{
    int i = 0;
    uint64_t resultset;
    char *str, *resultstr;

    TWOPARAMS("map");
    ONEQUOTE("map");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case LIST_:
        env->dump1
            = newnode(env, LIST_, nodevalue(SAVED2), env->dump1); /* step old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head new */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last new */
        while (DMP1) {
            env->stck = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            CHECKSTACK("map");
            if (!DMP2) { /* first */
                DMP2 = NEWNODE(nodetype(env->stck), nodevalue(env->stck), 0);
                DMP3 = DMP2;
            } else { /* further */
                nextnode1(DMP3)
                    = NEWNODE(nodetype(env->stck), nodevalue(env->stck), 0);
                DMP3 = nextnode1(DMP3);
            }
            DMP1 = nextnode1(DMP1);
        }
        env->stck = LIST_NEWNODE(DMP2, SAVED3);
        POP(env->dump3);
        POP(env->dump2);
        POP(env->dump1);
        break;
    case STRING_:
        resultstr = GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (str = nodevalue(SAVED2).str; *str != '\0'; str++) {
            env->stck = CHAR_NEWNODE(*str, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            CHECKSTACK("map");
            resultstr[i++] = nodevalue(env->stck).num;
        }
        resultstr[i] = '\0';
        env->stck = STRING_NEWNODE(resultstr, SAVED3);
        break;
    case SET_:
        for (resultset = i = 0; i < SETSIZE; i++)
            if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                CHECKSTACK("map");
                resultset |= ((int64_t)1 << nodevalue(env->stck).num);
            }
        env->stck = SET_NEWNODE(resultset, SAVED3);
        break;
    default:
        BADAGGREGATE("map");
    }
    POP(env->dump);
}
#endif
