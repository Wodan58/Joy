/*
    module  : treegenrec.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef TREEGENREC_C
#define TREEGENREC_C

/**
2910  treegenrec  :  T [O1] [O2] [C]  ->  ...
T is a tree. If T is a leaf, executes O1.
Else executes O2 and then [[[O1] [O2] C] treegenrec] C.
*/
PRIVATE void treegenrecaux(pEnv env)
{
    if (nodetype(nextnode1(env->stck)) == LIST_) {
        SAVESTACK; /* begin DIP        */
        POP(env->stck);
        exeterm(env,
            nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis); /*        [O2] */
        GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));
        POP(env->dump); /*   end DIP        */
        NULLARY(LIST_NEWNODE, ANON_FUNCT_NEWNODE(treegenrecaux, NULL));
        cons_(env);
        exeterm(env, nextnode2(nodevalue(nodevalue(env->stck).lis).lis));
    } /*        [C]        */
    else {
        env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
        POP(env->stck);
        exeterm(env, nodevalue(DMP1).lis);
        POP(env->dump1);
    }
}

PRIVATE void treegenrec_(pEnv env)
{ /* T [O1] [O2] [C]        */
    FOURPARAMS("treegenrec");
    THREEQUOTES("treegenrec");
    cons_(env);
    cons_(env);
    treegenrecaux(env);
}
#endif
