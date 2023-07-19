/*
    module  : split.c
    version : 1.4
    date    : 07/19/23
*/
#ifndef SPLIT_C
#define SPLIT_C

/**
2860  split  :  A [B]  ->  A1 A2
Uses test B to split aggregate A into sametype aggregates A1 and A2.
*/
PRIVATE void split_(pEnv env)
{
    TWOPARAMS("split");
    ONEQUOTE("split");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_: {
        int j;
        uint64_t yes_set = 0, no_set = 0;
        for (j = 0; j < SETSIZE; j++) {
            if (nodevalue(SAVED2).set & ((int64_t)1 << j)) {
                env->stck = INTEGER_NEWNODE(j, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                CHECKSTACK("split");
                if (nodevalue(env->stck).num)
                    yes_set |= ((int64_t)1 << j);
                else
                    no_set |= ((int64_t)1 << j);
            }
        }
        env->stck = SET_NEWNODE(yes_set, SAVED3);
        NULLARY(SET_NEWNODE, no_set);
        break;
    }
    case STRING_: {
        char *s, *yesstring, *nostring;
        int yesptr = 0, noptr = 0;
        yesstring = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        nostring = (char *)GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((int64_t)*s, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            CHECKSTACK("split");
            if (nodevalue(env->stck).num)
                yesstring[yesptr++] = *s;
            else
                nostring[noptr++] = *s;
        }
        yesstring[yesptr] = '\0';
        nostring[noptr] = '\0';
        env->stck = STRING_NEWNODE(yesstring, SAVED3);
        NULLARY(STRING_NEWNODE, nostring);
        break;
    }
    case LIST_: {
        env->dump1
            = newnode(env, LIST_, nodevalue(SAVED2), env->dump1); /* step old */
        env->dump2 = LIST_NEWNODE(0L, env->dump2); /* head true */
        env->dump3 = LIST_NEWNODE(0L, env->dump3); /* last true */
        env->dump4 = LIST_NEWNODE(0L, env->dump4); /* head false */
        env->dump5 = LIST_NEWNODE(0L, env->dump5); /* last false */
        while (DMP1) {
            env->stck = newnode(env, nodetype(DMP1), nodevalue(DMP1), SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            CHECKSTACK("split");
            if (nodevalue(env->stck).num) /* pass */
                if (!DMP2) { /* first */
                    DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP3 = DMP2;
                } else { /* further */
                    nextnode1(DMP3)
                        = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP3 = nextnode1(DMP3);
                }
            else /* fail */
                if (!DMP4) { /* first */
                    DMP4 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP5 = DMP4;
                } else { /* further */
                    nextnode1(DMP5)
                        = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP5 = nextnode1(DMP5);
                }
            DMP1 = nextnode1(DMP1);
        }
        env->stck = LIST_NEWNODE(DMP2, SAVED3);
        NULLARY(LIST_NEWNODE, DMP4);
        POP(env->dump5);
        POP(env->dump4);
        POP(env->dump3);
        POP(env->dump2);
        POP(env->dump1);
        break;
    }
    default:
        BADAGGREGATE("split");
    }
    POP(env->dump);
}
#endif
