/*
    module  : split.c
    version : 1.14
    date    : 11/11/24
*/
#ifndef SPLIT_C
#define SPLIT_C

#include "boolean.h"

/**
Q1  OK  2840  split  :  A [B]  ->  A1 A2
Uses test B to split aggregate A into sametype aggregates A1 and A2.
*/
void split_(pEnv env)
{
    Index temp;
    uint64_t yes_set = 0, no_set = 0;
    char *str, *yesstring, *nostring;
    int i = 0, yesptr = 0, noptr = 0, result = 0;

    TWOPARAMS("split");
    ONEQUOTE("split");
    SAVESTACK;
    switch (nodetype(SAVED2)) {
    case SET_:
	for (; i < SETSIZE; i++)
	    if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {
		env->stck = INTEGER_NEWNODE(i, SAVED3);
		exeterm(env, nodevalue(SAVED1).lis);
		CHECKSTACK("split");
		result = get_boolean(env, env->stck);
		if (result)
		    yes_set |= ((int64_t)1 << i);
		else
		    no_set |= ((int64_t)1 << i);
	    }
	env->stck = SET_NEWNODE(yes_set, SAVED3);
	NULLARY(SET_NEWNODE, no_set);
	break;
    case STRING_:
	yesstring = malloc(nodeleng(SAVED2) + 1);
	nostring = malloc(nodeleng(SAVED2) + 1);
	for (str = strdup((char *)&nodevalue(SAVED2)); str[i]; i++) {
	    env->stck = CHAR_NEWNODE(str[i], SAVED3);
	    exeterm(env, nodevalue(SAVED1).lis);
	    CHECKSTACK("split");
	    result = get_boolean(env, env->stck);
	    if (result)
		yesstring[yesptr++] = str[i];
	    else
		nostring[noptr++] = str[i];
	}
	yesstring[yesptr] = 0;
	nostring[noptr] = 0;
	env->stck = STRING_NEWNODE(yesstring, SAVED3);
	NULLARY(STRING_NEWNODE, nostring);
	free(str);
	free(nostring);
	free(yesstring);
	break;
    case LIST_:
	env->dump1 = LIST_NEWNODE(nodevalue(SAVED2).lis, env->dump1);
	env->dump2 = LIST_NEWNODE(0, env->dump2);	/* head true */
	env->dump3 = LIST_NEWNODE(0, env->dump3);	/* last true */
	env->dump4 = LIST_NEWNODE(0, env->dump4);	/* head false */
	env->dump5 = LIST_NEWNODE(0, env->dump5);	/* last false */
	for (; DMP1; DMP1 = nextnode1(DMP1)) {
	    env->stck = newnode2(env, DMP1, SAVED3);
	    exeterm(env, nodevalue(SAVED1).lis);
	    CHECKSTACK("split");
	    temp = newnode2(env, DMP1, 0);
	    result = get_boolean(env, env->stck);
	    if (result) {			/* pass */
		if (!DMP2) {			/* first */
		    DMP2 = temp;
		    DMP3 = DMP2;
		} else {			/* further */
		    nextnode1(DMP3) = temp;
		    DMP3 = nextnode1(DMP3);
		}
	    } else {				/* fail */
		if (!DMP4) {			/* first */
		    DMP4 = temp;
		    DMP5 = DMP4;
		} else {			/* further */
		    nextnode1(DMP5) = temp;
		    DMP5 = nextnode1(DMP5);
		}
	    }
	}
	env->stck = LIST_NEWNODE(DMP2, SAVED3);
	NULLARY(LIST_NEWNODE, DMP4);
	POP(env->dump5);
	POP(env->dump4);
	POP(env->dump3);
	POP(env->dump2);
	POP(env->dump1);
	break;
    default:
	BADAGGREGATE("split");
    }
    POP(env->dump);
}
#endif
