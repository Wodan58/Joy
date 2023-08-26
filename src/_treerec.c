/*
    module  : _treerec.c
    version : 1.1
    date    : 08/26/23
*/
#ifndef _TREEREC_C
#define _TREEREC_C

/**
OK 3180  (treerec)  :  T [O] [C]  ->  ...
T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C.
*/
PRIVATE void _treerec_(pEnv env)
{
    Index temp;

    if (nodetype(nextnode1(env->stck)) == LIST_) {
	temp = ANON_FUNCT_NEWNODE(_treerec_, 0);
	NULLARY(LIST_NEWNODE, temp);
	cons_(env); /*  D  [[[O] C] ANON_FUNCT_]	*/
	exeterm(env, nextnode1(nodevalue(nodevalue(env->stck).lis).lis));
    } else {
	env->dump1 = newnode(env, LIST_, nodevalue(env->stck), env->dump1);
	POP(env->stck);
	exeterm(env, nodevalue(DMP1).lis);
	POP(env->dump1);
    }
}
#endif
