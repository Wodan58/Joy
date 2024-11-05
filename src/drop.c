/*
    module  : drop.c
    version : 1.15
    date    : 10/28/24
*/
#ifndef DROP_C
#define DROP_C

/**
Q0  OK  2130  drop  :  A N  ->  B
Aggregate B is the result of deleting the first N elements of A.
*/
void drop_(pEnv env)
{
    char *str;
    Index list;
    uint64_t set;
    int i = 0, n;

    TWOPARAMS("drop");
    POSITIVEINDEX(env->stck, "drop");
    n = nodevalue(env->stck).num;
    POP(env->stck);
    switch (nodetype(env->stck)) {
    case SET_:
	for (set = 0; i < SETSIZE; i++)
	    if (nodevalue(env->stck).set & ((int64_t)1 << i)) {
		if (n < 1)
		    set |= ((int64_t)1 << i);
		else
		    n--;
	    }
	UNARY(SET_NEWNODE, set);
	break;
    case STRING_:
	str = GETSTRING(env->stck);
	while (n-- > 0 && str[i])
	    i++;
#ifdef NOBDW
	str = strdup(str + i);
	UNARY(STRING_NEWNODE, str);
	free(str);
#else
	UNARY(STRING_NEWNODE, GC_strdup(str + i));
#endif
	break;
    case LIST_:
	list = nodevalue(env->stck).lis;
	while (n-- > 0 && list)
	    list = nextnode1(list);
	UNARY(LIST_NEWNODE, list);
	break;
    default:
	BADAGGREGATE("drop");
    }
}
#endif
