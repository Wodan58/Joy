/*
    module  : helpdetail.c
    version : 1.17
    date    : 09/17/24
*/
#ifndef HELPDETAIL_C
#define HELPDETAIL_C

/**
Q0  IGNORE_POP  2920  helpdetail  :  [ S1 S2 .. ]  ->
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
    for (n = nodevalue(env->stck).lis; n; n = nextnode1(n)) {
	switch (op = nodetype(n)) {
	case USR_:
	    ent = vec_at(env->symtab, nodevalue(n).ent);
	    printf("%s  ==\n    ", ent.name);
	    writeterm(env, ent.u.body, stdout);
	    printf("\n\n");
	    continue;

	case ANON_FUNCT_:
	    op = operindex(env, nodevalue(n).proc);
	    break;

	case BOOLEAN_:
	    op = nodevalue(n).num ? operindex(env, true_)
				  : operindex(env, false_);
	    break;

	case INTEGER_:
	    if (nodevalue(n).num == MAXINT_)
		op = operindex(env, maxint_);
	    break;

	case FILE_:
	    if (nodevalue(n).fil == stdin)
		op = operindex(env, stdin_);
	    else if (nodevalue(n).fil == stdout)
		op = operindex(env, stdout_);
	    else if (nodevalue(n).fil == stderr)
		op = operindex(env, stderr_);
	    break;
	}
	printf("%s\t:  %s.\n%s\n", optable[op].name, optable[op].messg1,
		optable[op].messg2);
	if (op <= BIGNUM_)
	    printf("\n");
    }
    POP(env->stck);
}
#endif
