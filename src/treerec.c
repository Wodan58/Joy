/*
    module  : treerec.c
    version : 1.5
    date    : 08/13/23
*/
#ifndef TREEREC_C
#define TREEREC_C

/**
OK 2900  treerec  :  T [O] [C]  ->  ...
T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C.
*/
PRIVATE void treerecaux(pEnv env)
{
    Index temp;
    if (nodetype(nextnode1(env->stck)) == LIST_) {
        temp = ANON_FUNCT_NEWNODE(treerecaux, 0);
        NULLARY(LIST_NEWNODE, temp);
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
