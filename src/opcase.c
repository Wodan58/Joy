/*
    module  : opcase.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef OPCASE_C
#define OPCASE_C

/**
Q0  OK  2090  opcase  :  X [..[X Xs]..]  ->  X [Xs]
Indexing on type of X, returns the list [Xs].
*/
void opcase_(pEnv env)
{
    int op;
    proc_t proc;
    Index n, temp;

    TWOPARAMS("opcase");
    LIST("opcase");
    n = nodevalue(env->stck).lis;
    CHECKEMPTYLIST(n, "opcase");
    if ((op = nodetype(nextnode1(env->stck))) == ANON_FUNCT_)
	proc = nodevalue(nextnode1(env->stck)).proc;
    while (nextnode1(n) && nodetype(n) == LIST_) {
	temp = nodevalue(n).lis;
	if (op == nodetype(temp)) {
	    if (op != ANON_FUNCT_ || proc == nodevalue(temp).proc)
		break;
	}
	n = nextnode1(n);
    }
    CHECKLIST(nodetype(n), "opcase");
    UNARY(LIST_NEWNODE,
	nextnode1(n) ? nextnode1(nodevalue(n).lis) : nodevalue(n).lis);
}
#endif
