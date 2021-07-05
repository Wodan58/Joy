/*
    module  : condnestrec.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef CONDNESTREC_C
#define CONDNESTREC_C

/**
2770  condnestrec  :  [ [C1] [C2] .. [D] ]  ->  ...
A generalisation of condlinrec.
Each [Ci] is of the form [[B] [R1] [R2] .. [Rn]] and [D] is of the form
[[R1] [R2] .. [Rn]]. Tries each B, or if all fail, takes the default [D].
For the case taken, executes each [Ri] but recurses between any two
consecutive [Ri] (n > 3 would be exceptional.)
*/
PRIVATE void condnestrecaux(pEnv env)
{
    int result = 0;
    env->dump1 = newnode(env, LIST_, nodevalue(SAVED1), env->dump1);
    env->dump2 = LIST_NEWNODE(env->stck, env->dump2);
    while (result == 0 && DMP1 != NULL && nextnode1(DMP1) != NULL) {
        env->stck = DMP2;
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        result = nodevalue(env->stck).num;
        if (!result)
            DMP1 = nextnode1(DMP1);
    }
    env->stck = DMP2;
    env->dump3 = LIST_NEWNODE(
        (result ? nextnode1(nodevalue(DMP1).lis) : nodevalue(DMP1).lis),
        env->dump3);
    exeterm(env, nodevalue(DMP3).lis);
    DMP3 = nextnode1(DMP3);
    while (DMP3 != NULL) {
        condnestrecaux(env);
        exeterm(env, nodevalue(DMP3).lis);
        DMP3 = nextnode1(DMP3);
    }
    POP(env->dump3);
    POP(env->dump2);
    POP(env->dump1);
}

PRIVATE void condnestrec_(pEnv env)
{
    ONEPARAM("condnestrec");
    LIST("condnestrec");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "condnestrec");
    SAVESTACK;
    env->stck = SAVED2;
    condnestrecaux(env);
    POP(env->dump);
}
#endif
