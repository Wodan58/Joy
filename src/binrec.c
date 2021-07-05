/*
    module  : binrec.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef BINREC_C
#define BINREC_C

/**
2750  binrec  :  [P] [T] [R1] [R2]  ->  ...
Executes P. If that yields true, executes T.
Else uses R1 to produce two intermediates, recurses on both,
then executes R2 to combine their results.
*/
PRIVATE void binrecaux(pEnv env)
{
    int result;
    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED4).lis);
    result = nodevalue(env->stck).num;
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
        exeterm(env, nodevalue(SAVED3).lis);
    else {
        exeterm(env, nodevalue(SAVED2).lis); /* split */
        env->dump2 = newnode(
            env, nodetype(env->stck), nodevalue(env->stck), env->dump2);
        POP(env->stck);
        binrecaux(env); /* first */
        GNULLARY(nodetype(env->dump2), nodevalue(env->dump2));
        POP(env->dump2);
        binrecaux(env); /* second */
        exeterm(env, nodevalue(SAVED1).lis);
    } /* combine */
}

PRIVATE void binrec_(pEnv env)
{
    FOURPARAMS("binrec");
    FOURQUOTES("binrec");
    SAVESTACK;
    env->stck = SAVED5;
    binrecaux(env);
    POP(env->dump);
}
#endif
