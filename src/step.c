/*
    module  : step.c
    version : 1.7
    date    : 03/21/24
*/
#ifndef STEP_C
#define STEP_C

/**
OK 2770  step  :  A [P]  ->  ...
Sequentially putting members of aggregate A onto stack,
executes P for each member of A.
*/
void step_(pEnv env)
{
    int i;
    char *str;

    TWOPARAMS("step");
    ONEQUOTE("step");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    switch (nodetype(SAVED2)) {
    case LIST_:
        env->dump1 = newnode(env, LIST_, nodevalue(SAVED2), env->dump1);
        while (DMP1) {
            GNULLARY(nodetype(DMP1), nodevalue(DMP1));
            exeterm(env, nodevalue(SAVED1).lis);
            DMP1 = nextnode1(DMP1);
        }
        POP(env->dump1);
        break;
    case STRING_:
        for (str = nodevalue(SAVED2).str; *str != '\0'; str++) {
            env->stck = CHAR_NEWNODE(*str, env->stck);
            exeterm(env, nodevalue(SAVED1).lis);
        }
        break;
    case SET_:
        for (i = 0; i < SETSIZE; i++)
            if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {
                env->stck = INTEGER_NEWNODE(i, env->stck);
                exeterm(env, nodevalue(SAVED1).lis);
            }
        break;
    default:
        BADAGGREGATE("step");
    }
    POP(env->dump);
}
#endif
