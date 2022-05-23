/*
    module  : step.c
    version : 1.2
    date    : 05/17/22
*/
#ifndef STEP_C
#define STEP_C

/**
2790  step  :  A [P]  ->  ...
Sequentially putting members of aggregate A onto stack,
executes P for each member of A.
*/
PRIVATE void step_(pEnv env)
{
    TWOPARAMS("step");
    ONEQUOTE("step");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    switch (nodetype(SAVED2)) {
    case LIST_: {
        env->dump1 = newnode(env, LIST_, nodevalue(SAVED2), env->dump1);
        while (DMP1) {
            GNULLARY(nodetype(DMP1), nodevalue(DMP1));
            exeterm(env, nodevalue(SAVED1).lis);
            DMP1 = nextnode1(DMP1);
        }
        POP(env->dump1);
        break;
    }
    case STRING_: {
        char *s;
        for (s = nodevalue(SAVED2).str; *s != '\0'; s++) {
            env->stck = CHAR_NEWNODE((long)*s, env->stck);
            exeterm(env, nodevalue(SAVED1).lis);
        }
        break;
    }
    case SET_: {
        int i;
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(SAVED2).set & ((long)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, env->stck);
                exeterm(env, nodevalue(SAVED1).lis);
            }
        break;
    }
    default:
        BADAGGREGATE("step");
    }
    POP(env->dump);
}
#endif
