/*
    module  : _genrec.c
    version : 1.1
    date    : 08/26/23
*/
#ifndef _GENREC_C
#define _GENREC_C

/**
OK 3170  (genrec)  :  [B] [T] [R1] [R2]  ->  ...
Executes B, if that yields true, executes T.
Else executes R1 and then [[[B] [T] [R1] R2] genrec] R2.
*/
PRIVATE void _genrec_(pEnv env)
{
    Index temp;
    int result;

    SAVESTACK;
    POP(env->stck);
    exeterm(env, nodevalue(nodevalue(SAVED1).lis).lis);		/*	[B]  */
    CHECKSTACK("genrec");
    result = nodevalue(env->stck).num;
    env->stck = SAVED2;
    if (result)
	exeterm(env,
	    nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis);	/*	[T]  */
    else {
	exeterm(env,
	    nodevalue(nextnode2(nodevalue(SAVED1).lis)).lis);	/*	[R1] */
	NULLARY(LIST_NEWNODE, nodevalue(SAVED1).lis);
	temp = ANON_FUNCT_NEWNODE(_genrec_, 0);
	NULLARY(LIST_NEWNODE, temp);
	cons_(env);
	exeterm(env, nextnode3(nodevalue(SAVED1).lis));		/*	[R2] */
    }
    POP(env->dump);
}
#endif
