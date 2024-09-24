/*
    module  : take.c
    version : 1.13
    date    : 09/17/24
*/
#ifndef TAKE_C
#define TAKE_C

/**
Q0  OK  2140  take  :  A N  ->  B
Aggregate B is the result of retaining just the first N elements of A.
*/
void take_(pEnv env)
{
    char *str;
    Index temp;
    int i = 0, n;
    uint64_t set;

    TWOPARAMS("take");
    POSITIVEINDEX(env->stck, "take");
    n = nodevalue(env->stck).num;
    POP(env->stck);
    switch (nodetype(env->stck)) {
    case SET_:
	for (set = 0; i < SETSIZE && n; i++)
	    if (nodevalue(env->stck).set & ((int64_t)1 << i)) {
		set |= ((int64_t)1 << i);
		n--;
	    }
	UNARY(SET_NEWNODE, set);
	break;
    case STRING_:
	if (n >= (int)nodeleng(env->stck))
	    return;	/* the old string unchanged */
	str = strdup((char *)&nodevalue(env->stck));
	str[n] = 0;	/* end the string */
	UNARY(STRING_NEWNODE, str);
	free(str);
	break;
    case LIST_:
	env->dump1 = LIST_NEWNODE(nodevalue(env->stck).lis, env->dump1);
	env->dump2 = LIST_NEWNODE(0, env->dump2);	/* head */
	env->dump3 = LIST_NEWNODE(0, env->dump3);	/* last */
	for (; DMP1 && n; DMP1 = nextnode1(DMP1), n--) {
	    temp = newnode2(env, DMP1, 0);
	    if (!DMP2) {				/* first */
		DMP2 = temp;
		DMP3 = DMP2;
	    } else {					/* further */
		nextnode1(DMP3) = temp;
		DMP3 = nextnode1(DMP3);
	    }
	}
	UNARY(LIST_NEWNODE, DMP2);
	POP(env->dump1);
	POP(env->dump2);
	POP(env->dump3);
	break;
    default:
	BADAGGREGATE("take");
    }
}
#endif
