/*
    module  : treegenrecaux.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef TREEGENRECAUX_C
#define TREEGENRECAUX_C

/**
Q1  OK  3250  #treegenrec  :  T [[O1] [O2] C]  ->  ...
T is a tree. If T is a leaf, executes O1.
Else executes O2 and then [[[O1] [O2] C] treegenrec] C.
*/
void treegenrecaux_(pEnv env)
{
    Index temp;

    if (nodetype(nextnode1(env->stck)) == LIST_) {
	SAVESTACK;	/* begin DIP */
	POP(env->stck);
	exeterm(env, nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis);/* [O2] */
	GNULLARY(SAVED1);
	POP(env->dump);	/* end DIP */
	temp = ANON_FUNCT_NEWNODE(treegenrecaux_, 0);
	NULLARY(LIST_NEWNODE, temp);
	cons_(env);
	exeterm(env, nextnode2(nodevalue(nodevalue(env->stck).lis).lis));
    } else {	/* [C] */
	env->dump1 = LIST_NEWNODE(nodevalue(env->stck).lis, env->dump1);
	POP(env->stck);
	exeterm(env, nodevalue(DMP1).lis);
	POP(env->dump1);
    }
}
#endif
