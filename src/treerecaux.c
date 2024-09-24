/*
    module  : treerecaux.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef TREERECAUX_C
#define TREERECAUX_C

/**
Q1  OK  3260  #treerec  :  T [[O] C]  ->  ...
T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C.
*/
void treerecaux_(pEnv env)
{
    Index temp;

    if (nodetype(nextnode1(env->stck)) == LIST_) {
	temp = ANON_FUNCT_NEWNODE(treerecaux_, 0);
	NULLARY(LIST_NEWNODE, temp);
	cons_(env); /*  D  [[[O] C] ANON_FUNCT_]  */
	exeterm(env, nextnode1(nodevalue(nodevalue(env->stck).lis).lis));
    } else {
	env->dump1 = LIST_NEWNODE(nodevalue(env->stck).lis, env->dump1);
	POP(env->stck);
	exeterm(env, nodevalue(DMP1).lis);
	POP(env->dump1);
    }
}
#endif
