/*
    module  : condlinrec.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef CONDLINREC_C
#define CONDLINREC_C

/**
2780  condlinrec  :  [ [C1] [C2] .. [D] ]  ->  ...
Each [Ci] is of the form [[B] [T]] or [[B] [R1] [R2]].
Tries each B. If that yields true and there is just a [T], executes T and exit.
If there are [R1] and [R2], executes R1, recurses, executes R2.
Subsequent case are ignored. If no B yields true, then [D] is used.
It is then of the form [[T]] or [[R1] [R2]]. For the former, executes T.
For the latter executes R1, recurses, executes R2.
*/
PRIVATE void condlinrecaux(pEnv env)
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
    if (result) {
        exeterm(env, nodevalue(nextnode1(nodevalue(DMP1).lis)).lis);
        if (nextnode2(nodevalue(DMP1).lis) != NULL) {
            condlinrecaux(env);
            exeterm(env, nodevalue(nextnode2(nodevalue(DMP1).lis)).lis);
        }
    } else {
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        if (nextnode1(nodevalue(DMP1).lis) != NULL) {
            condlinrecaux(env);
            exeterm(env, nodevalue(nextnode1(nodevalue(DMP1).lis)).lis);
        }
    }
    POP(env->dump2);
    POP(env->dump1);
}

PRIVATE void condlinrec_(pEnv env)
{
    ONEPARAM("condlinrec");
    LIST("condlinrec");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "condlinrec");
    SAVESTACK;
    env->stck = SAVED2;
    condlinrecaux(env);
    POP(env->dump);
}
#endif
