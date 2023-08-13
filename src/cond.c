/*
    module  : cond.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef COND_C
#define COND_C

/**
OK 2710  cond  :  [..[[Bi] Ti]..[D]]  ->  ...
Tries each Bi. If that yields true, then executes Ti and exits.
If no Bi yields true, executes default D.
*/
PRIVATE void cond_(pEnv env)
{
    Index my_dump;
    int result = 0;

    ONEPARAM("cond");
    LIST("cond");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "cond");
    /* must check for QUOTES in list */
    for (my_dump = nodevalue(env->stck).lis; nextnode1(my_dump);
         my_dump = nextnode1(my_dump))
        CHECKLIST(nodetype(nodevalue(my_dump).lis), "cond");
    SAVESTACK;
    env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
    while (!result && DMP1 && nextnode1(DMP1)) {
        env->stck = SAVED2;
        exeterm(env, nodevalue(nodevalue(DMP1).lis).lis);
        result = nodevalue(env->stck).num;
        if (!result)
            DMP1 = nextnode1(DMP1);
    }
    env->stck = SAVED2;
    if (result)
        exeterm(env, nextnode1(nodevalue(DMP1).lis));
    else
        exeterm(env, nodevalue(DMP1).lis); /* default */
    POP(env->dump1);
    POP(env->dump);
}
#endif
