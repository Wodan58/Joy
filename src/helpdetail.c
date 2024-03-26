/*
    module  : helpdetail.c
    version : 1.15
    date    : 03/23/24
*/
#ifndef HELPDETAIL_C
#define HELPDETAIL_C

/**
IGNORE_POP  2920  helpdetail  :  [ S1 S2 .. ]  ->
[IMPURE] Gives brief help on each symbol S in the list.
*/
void helpdetail_(pEnv env)
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
	    writeterm(env, ent.u.body, stdout);
	    printf("\n\n");
	} else {
	    if (op == ANON_FUNCT_)    
		op = operindex(env, nodevalue(n).proc);
	    else if (op == BOOLEAN_)
		op = nodevalue(n).num ? operindex(env, true_)
				      : operindex(env, false_);
	    else if (op == INTEGER_ && nodevalue(n).num == MAXINT_)
		op = operindex(env, maxint_);
	    else if (op == FILE_) {
		if (nodevalue(n).fil == stdin)
		    op = operindex(env, stdin_);
		else if (nodevalue(n).fil == stdout)
		    op = operindex(env, stdout_);
		else if (nodevalue(n).fil == stderr)
		    op = operindex(env, stderr_);
	    }
	    printf("%s\t:  %s.\n%s\n", optable[op].name,
		optable[op].messg1, optable[op].messg2);
	    if (op <= BIGNUM_)
		printf("\n");
	}
	n = nextnode1(n);
    }
    POP(env->stck);
}
#endif
