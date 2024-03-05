/*
    module  : helpdetail.c
    version : 1.13
    date    : 03/05/24
*/
#ifndef HELPDETAIL_C
#define HELPDETAIL_C

/**
OK 2920  helpdetail  :  [ S1 S2 .. ]  ->
[IMPURE] Gives brief help on each symbol S in the list.
*/
PRIVATE void helpdetail_(pEnv env)
{
    int op;
    Index n;
    Entry ent;

    ONEPARAM("HELP");
    LIST("HELP");
    if (env->ignore) {
	POP(env->stck);
	return;
    }
    printf("\n");
    n = nodevalue(env->stck).lis;
    while (n) {
	if ((op = nodetype(n)) == USR_) {
	    ent = vec_at(env->symtab, nodevalue(n).ent);
	    printf("%s  ==\n    ", ent.name);
	    writeterm(env, ent.u.body, stdout);
	    printf("\n\n");
	} else {
	    if (op == ANON_FUNCT_)    
		op = operindex(env, nodevalue(n).proc);
	    else if (op == BOOLEAN_)
		op = nodevalue(n).num ? operindex(env, true_)
				      : operindex(env, false_);
	    else if (op == INTEGER_ && nodevalue(n).num == MAXINT)
		op = operindex(env, maxint_);
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
