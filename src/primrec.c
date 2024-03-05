/*
    module  : primrec.c
    version : 1.6
    date    : 03/05/24
*/
#ifndef PRIMREC_C
#define PRIMREC_C

/**
OK 2820  primrec  :  X [I] [C]  ->  R
Executes I to obtain an initial value R0.
For integer X uses increasing positive integers to X, combines by C for new R.
For aggregate X uses successive members and combines by C for new R.
*/
PRIVATE void primrec_(pEnv env)
{
    char *str;
    int i, n = 0;
    uint64_t set;

    THREEPARAMS("primrec");
    TWOQUOTES("primrec");
    SAVESTACK;
    env->stck = nextnode3(env->stck);
    switch (nodetype(SAVED3)) {
    case LIST_:
        env->dump1 = newnode(env, LIST_, nodevalue(SAVED3), env->dump1);
        while (DMP1) {
            env->stck
                = newnode(env, nodetype(DMP1), nodevalue(DMP1), env->stck);
            DMP1 = nextnode1(DMP1);
            n++;
        }
        POP(env->dump1);
        break;
    case STRING_:
        for (str = nodevalue(SAVED3).str; *str != '\0'; str++) {
            env->stck = CHAR_NEWNODE(*str, env->stck);
            n++;
        }
        break;
    case SET_:
        set = nodevalue(SAVED3).set;
        for (i = 0; i < SETSIZE; i++)
            if (set & ((int64_t)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, env->stck);
                n++;
            }
        break;
    case INTEGER_:
        for (i = nodevalue(SAVED3).num; i > 0; i--) {
            env->stck = INTEGER_NEWNODE(i, env->stck);
            n++;
        }
        break;
    default:
        BADDATA("primrec");
    }
    exeterm(env, nodevalue(SAVED2).lis);
    for (i = 1; i <= n; i++)
        exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
