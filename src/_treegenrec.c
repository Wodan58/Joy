/*
    module  : _treegenrec.c
    version : 1.2
    date    : 09/04/23
*/
#ifndef _TREEGENREC_C
#define _TREEGENREC_C

/**
OK 3150  (treegenrec)  :  T [O1] [O2] [C]  ->  ...
T is a tree. If T is a leaf, executes O1.
Else executes O2 and then [[[O1] [O2] C] treegenrec] C.
*/
PRIVATE void _treegenrec_(pEnv env)
{
    Index temp;

    if (nodetype(nextnode1(env->stck)) == LIST_) {
	SAVESTACK; /* begin DIP	*/
	POP(env->stck);
	exeterm(env,
	    nodevalue(nextnode1(nodevalue(SAVED1).lis)).lis); /*	[O2] */
	GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));
	POP(env->dump); /*   end DIP	*/
	temp = ANON_FUNCT_NEWNODE(_treegenrec_, 0);
	NULLARY(LIST_NEWNODE, temp);
	cons_(env);
	exeterm(env, nextnode2(nodevalue(nodevalue(env->stck).lis).lis));
    } /*	[C]	*/
    else {
	env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
	POP(env->stck);
	exeterm(env, nodevalue(DMP1).lis);
	POP(env->dump1);
    }
}
#endif
