/*
    module  : helpdetail.c
    version : 1.8
    date    : 08/26/23
*/
#ifndef HELPDETAIL_C
#define HELPDETAIL_C

/**
OK 2940  helpdetail  :  [ S1 S2 .. ]  ->
Gives brief help on each symbol S in the list.
*/
PRIVATE void helpdetail_(pEnv env)
{
    int op;
    Index n;
    Entry ent;

    ONEPARAM("HELP");
    LIST("HELP");
    printf("\n");
    n = nodevalue(env->stck).lis;
    while (n) {
	if ((op = nodetype(n)) == USR_) {
	    ent = vec_at(env->symtab, nodevalue(n).ent);
	    printf("%s  ==\n    ", ent.name);
	    writeterm(env, ent.u.body);
	    printf("\n\n");
	} else {
	    if (op == ANON_FUNCT_)    
		op = operindex(nodevalue(n).proc);
	    else if (op == BOOLEAN_)
		op = nodevalue(n).num ? operindex(true_) : operindex(false_);
	    else if (op == INTEGER_ && nodevalue(n).num == MAXINT)
		op = operindex(maxint_);
	    printf("%s\t:  %s.\n%s\n", optable[op].name,
		optable[op].messg1, optable[op].messg2);
	    if (op <= FILE_)
		printf("\n");
	}
	n = nextnode1(n);
    }
    POP(env->stck);
}
#endif
