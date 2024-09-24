/*
    module  : concat.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef CONCAT_C
#define CONCAT_C

/**
Q0  OK  2150  concat  :  S T  ->  U
Sequence U is the concatenation of sequences S and T.
*/
void concat_(pEnv env)
{
    char *str;
    Index temp;
    size_t leng;

    TWOPARAMS("concat");
    SAME2TYPES("concat");
    switch (nodetype(env->stck)) {
    case SET_:
	BINARY(SET_NEWNODE, nodevalue(nextnode1(env->stck)).set |
			nodevalue(env->stck).set);
	break;
    case STRING_:
	leng = nodeleng(nextnode1(env->stck)) + nodeleng(env->stck) + 1;
	str = malloc(leng);
	snprintf(str, leng, "%s%s", (char *)&nodevalue(nextnode1(env->stck)),
			(char *)&nodevalue(env->stck));
	BINARY(STRING_NEWNODE, str);
	free(str);
	break;
    case LIST_:
	if (!nodevalue(nextnode1(env->stck)).lis) {
	    BINARY(LIST_NEWNODE, nodevalue(env->stck).lis);
	    return;
	}
	env->dump1 = LIST_NEWNODE(nodevalue(nextnode1(env->stck)).lis,
			env->dump1);			/* old */
	env->dump2 = LIST_NEWNODE(0, env->dump2);	/* head */
	env->dump3 = LIST_NEWNODE(0, env->dump3);	/* last */
	for (; DMP1; DMP1 = nextnode1(DMP1)) {
	    temp = newnode2(env, DMP1, 0);
	    if (!DMP2) {	/* first */
		DMP2 = temp;
		DMP3 = DMP2;
	    } else {		/* further */
		nextnode1(DMP3) = temp;
		DMP3 = nextnode1(DMP3);
	    }
	}
	nextnode1(DMP3) = nodevalue(env->stck).lis;
	BINARY(LIST_NEWNODE, DMP2);
	POP(env->dump1);
	POP(env->dump2);
	POP(env->dump3);
	break;
    default:
	BADAGGREGATE("concat");
    }
}
#endif
