/*
    module  : genrec.c
    version : 1.5
    date    : 06/29/22
*/
#ifndef GENREC_C
#define GENREC_C

/**
2760  genrec  :  [B] [T] [R1] [R2]  ->  ...
Executes B, if that yields true, executes T.
Else executes R1 and then [[[B] [T] [R1] R2] genrec] R2.
*/
PRIVATE void genrecaux(pEnv env)
{
    Index temp;
    int result;
    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(nodevalue(SAVED1).lis).lis); /*        [I]        */
    CHECKSTACK("genrec");
    result = nodevalue(env->stck).num;
    env->stck = SAVED2;
    if (result)
        exeterm(env,
            nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis); /*        [T] */
    else {
        exeterm(env,
            nodevalue(nextnode2(nodevalue(SAVED1).lis)).lis); /*        [R1] */
        NULLARY(LIST_NEWNODE, nodevalue(SAVED1).lis);
        temp = ANON_FUNCT_NEWNODE(genrecaux, 0);
        NULLARY(LIST_NEWNODE, temp);
        cons_(env);
        exeterm(env, nextnode3(nodevalue(SAVED1).lis));
    } /*   [R2]        */
    POP(env->dump);
}

PRIVATE void genrec_(pEnv env)
{
    FOURPARAMS("genrec");
    FOURQUOTES("genrec");
    cons_(env);
    cons_(env);
    cons_(env);
    genrecaux(env);
}
#endif
