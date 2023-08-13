/*
    module  : treestep.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef TREESTEP_C
#define TREESTEP_C

/**
OK 2890  treestep  :  T [P]  ->  ...
Recursively traverses leaves of tree T, executes P for each leaf.
*/
PRIVATE void treestepaux(pEnv env, Index item)
{
    if (nodetype(item) != LIST_) {
        GNULLARY(nodetype(item), nodevalue(item));
        exeterm(env, nodevalue(SAVED1).lis);
    } else {
        env->dump1 = newnode(env, LIST_, nodevalue(item), env->dump1);
        while (DMP1) {
            treestepaux(env, DMP1);
            DMP1 = nextnode1(DMP1);
        }
        POP(env->dump1);
    }
}

PRIVATE void treestep_(pEnv env)
{
    TWOPARAMS("treestep");
    ONEQUOTE("treestep");
    SAVESTACK;
    env->stck = SAVED3;
    treestepaux(env, SAVED2);
    POP(env->dump);
}
#endif
