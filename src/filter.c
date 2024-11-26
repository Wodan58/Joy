/*
    module  : filter.c
    version : 1.12
    date    : 11/11/24
*/
#ifndef FILTER_C
#define FILTER_C

#include "boolean.h"

/**
Q1  OK  2830  filter  :  A [B]  ->  A1
Uses test B to filter aggregate A producing sametype aggregate A1.
*/
void filter_(pEnv env)
{
    char *str;
    Index temp;
    uint64_t set;
    int i = 0, j = 0, result = 0;

    TWOPARAMS("filter");
    ONEQUOTE("filter");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_:
	for (set = 0; i < SETSIZE; i++)
	    if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {
		env->stck = INTEGER_NEWNODE(i, SAVED3);
		exeterm(env, nodevalue(SAVED1).lis);
		CHECKSTACK("filter");
		result = get_boolean(env, env->stck);
		if (result)
		    set |= ((int64_t)1 << i);
	    }
	env->stck = SET_NEWNODE(set, SAVED3);
	break;
    case STRING_:
	for (str = strdup((char *)&nodevalue(SAVED2)); str[i]; i++) {
	    env->stck = CHAR_NEWNODE(str[i], SAVED3);
	    exeterm(env, nodevalue(SAVED1).lis);
	    CHECKSTACK("filter");
	    result = get_boolean(env, env->stck);
	    if (result)
		str[j++] = str[i];
	}
	str[j] = 0;
	env->stck = STRING_NEWNODE(str, SAVED3);
	free(str);
	break;
    case LIST_:
	env->dump1 = LIST_NEWNODE(nodevalue(SAVED2).lis, env->dump1);
	env->dump2 = LIST_NEWNODE(0, env->dump2);	/* head new */
	env->dump3 = LIST_NEWNODE(0, env->dump3);	/* last new */
	for (; DMP1; DMP1 = nextnode1(DMP1)) {
	    env->stck = newnode2(env, DMP1, SAVED3);
	    exeterm(env, nodevalue(SAVED1).lis);
	    CHECKSTACK("filter");
	    result = get_boolean(env, env->stck);
	    if (result) {			/* test */
		temp = newnode2(env, DMP1, 0);
		if (!DMP2) {			/* first */
		    DMP2 = temp;
		    DMP3 = DMP2;
		} else {			/* further */
		    nextnode1(DMP3) = temp;
		    DMP3 = nextnode1(DMP3);
		}
	    }
	}
	env->stck = LIST_NEWNODE(DMP2, SAVED3);
	POP(env->dump3);
	POP(env->dump2);
	POP(env->dump1);
	break;
    default:
	BADAGGREGATE("filter");
    }
    POP(env->dump);
}
#endif
