/*
    module  : split.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef SPLIT_C
#define SPLIT_C

/**
OK 2840  split  :  A [B]  ->  A1 A2
Uses test B to split aggregate A into sametype aggregates A1 and A2.
*/
void split_(pEnv env)
{
    int i, yesptr = 0, noptr = 0;
    uint64_t yes_set = 0, no_set = 0;
    char *str, *yesstring, *nostring;

    TWOPARAMS("split");
    ONEQUOTE("split");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_:
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, SAVED3);
                exeterm(env, nodevalue(SAVED1).lis);
                CHECKSTACK("split");
                if (nodevalue(env->stck).num)
                    yes_set |= ((int64_t)1 << i);
                else
                    no_set |= ((int64_t)1 << i);
            }
        env->stck = SET_NEWNODE(yes_set, SAVED3);
        NULLARY(SET_NEWNODE, no_set);
        break;
    case STRING_:
        yesstring = GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        nostring = GC_malloc_atomic(strlen(nodevalue(SAVED2).str) + 1);
        for (str = nodevalue(SAVED2).str; *str != '\0'; str++) {
            env->stck = CHAR_NEWNODE(*str, SAVED3);
            exeterm(env, nodevalue(SAVED1).lis);
            CHECKSTACK("split");
            if (nodevalue(env->stck).num)
                yesstring[yesptr++] = *str;
            else
                nostring[noptr++] = *str;
        }
        yesstring[yesptr] = '\0';
        nostring[noptr] = '\0';
        env->stck = STRING_NEWNODE(yesstring, SAVED3);
        NULLARY(STRING_NEWNODE, nostring);
        break;
    case LIST_:
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
            if (nodevalue(env->stck).num) { /* pass */
                if (!DMP2) { /* first */
                    DMP2 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP3 = DMP2;
                } else { /* further */
                    nextnode1(DMP3)
                        = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP3 = nextnode1(DMP3);
                }
	    } else { /* fail */
                if (!DMP4) { /* first */
                    DMP4 = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP5 = DMP4;
                } else { /* further */
                    nextnode1(DMP5)
                        = NEWNODE(nodetype(DMP1), nodevalue(DMP1), 0);
                    DMP5 = nextnode1(DMP5);
                }
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
    default:
        BADAGGREGATE("split");
    }
    POP(env->dump);
}
#endif
