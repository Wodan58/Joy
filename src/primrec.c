/*
    module  : primrec.c
    version : 1.2
    date    : 05/17/22
*/
#ifndef PRIMREC_C
#define PRIMREC_C

/**
2840  primrec  :  X [I] [C]  ->  R
Executes I to obtain an initial value R0.
For integer X uses increasing positive integers to X, combines by C for new R.
For aggregate X uses successive members and combines by C for new R.
*/
PRIVATE void primrec_(pEnv env)
{
    int n = 0;
    int i;
    THREEPARAMS("primrec");
    TWOQUOTES("primrec");
    SAVESTACK;
    env->stck = nextnode3(env->stck);
    switch (nodetype(SAVED3)) {
    case LIST_: {
        env->dump1 = newnode(env, LIST_, nodevalue(SAVED3), env->dump1);
        while (DMP1) {
            env->stck
                = newnode(env, nodetype(DMP1), nodevalue(DMP1), env->stck);
            DMP1 = nextnode1(DMP1);
            n++;
        }
        POP(env->dump1);
        break;
    }
    case STRING_: {
        char *s;
        for (s = nodevalue(SAVED3).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long)*s, env->stck);
            n++;
        }
        break;
    }
    case SET_: {
        int j;
        long set = nodevalue(SAVED3).set;
        for (j = 0; j < SETSIZE; j++)
            if (set & ((long)1 << j)) {
                env->stck = INTEGER_NEWNODE(j, env->stck);
                n++;
            }
        break;
    }
    case INTEGER_: {
        long j;
        for (j = nodevalue(SAVED3).num; j > 0; j--) {
            env->stck = INTEGER_NEWNODE(j, env->stck);
            n++;
        }
        break;
    }
    default:
        BADDATA("primrec");
    }
    exeterm(env, nodevalue(SAVED2).lis);
    for (i = 1; i <= n; i++)
        exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
