/*
    module  : treerec.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef TREEREC_C
#define TREEREC_C

/**
2900  treerec  :  T [O] [C]  ->  ...
T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C.
*/
PRIVATE void treerecaux(pEnv env)
{
    if (nodetype(nextnode1(env->stck)) == LIST_) {
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(treerecaux, NULL));
        cons_(env); /*  D  [[[O] C] ANON_FUNCT_]        */
        exeterm(env, nextnode1(nodevalue(nodevalue(env->stck).lis).lis));
    } else {
        env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
        POP(env->stck);
        exeterm(env, nodevalue(DMP1).lis);
        POP(env->dump1);
    }
}

PRIVATE void treerec_(pEnv env)
{
    THREEPARAMS("treerec");
    TWOQUOTES("treerec");
    cons_(env);
    treerecaux(env);
}
#endif
