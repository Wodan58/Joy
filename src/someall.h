/*
    module  : someall.h
    version : 1.5
    date    : 02/01/24
*/
#ifndef SOMEALL_H
#define SOMEALL_H

#define SOMEALL(PROCEDURE, NAME, INITIAL)				\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	int j, result = INITIAL;					\
	char *str;							\
	TWOPARAMS(NAME);						\
	ONEQUOTE(NAME);							\
	SAVESTACK;							\
	switch (nodetype(SAVED2)) {					\
	case SET_:							\
	    for (j = 0; j < SETSIZE && result == INITIAL; j++)		\
		if (nodevalue(SAVED2).set & ((int64_t)1 << j)) {	\
		    env->stck = INTEGER_NEWNODE(j, SAVED3);		\
		    exeterm(env, nodevalue(SAVED1).lis);		\
		    CHECKSTACK(NAME);					\
		    if (nodevalue(env->stck).num != INITIAL)		\
			result = 1 - INITIAL;				\
		}							\
	    break;							\
	case STRING_:							\
	    for (str = nodevalue(SAVED2).str; *str != '\0' &&		\
				result == INITIAL; str++) {		\
		env->stck = CHAR_NEWNODE(*str, SAVED3);			\
		exeterm(env, nodevalue(SAVED1).lis);			\
		CHECKSTACK(NAME);					\
		if (nodevalue(env->stck).num != INITIAL)		\
		    result = 1 - INITIAL;				\
	    }								\
	    break;							\
	case LIST_:							\
	    env->dump1 = newnode(env, LIST_, nodevalue(SAVED2),		\
				 env->dump1);				\
	    while (DMP1 && result == INITIAL) {				\
		env->stck = newnode(env, nodetype(DMP1),		\
				    nodevalue(DMP1), SAVED3);		\
		exeterm(env, nodevalue(SAVED1).lis);			\
		CHECKSTACK(NAME);					\
		if (nodevalue(env->stck).num != INITIAL)		\
		    result = 1 - INITIAL;				\
		DMP1 = nextnode1(DMP1);					\
	    }								\
	    POP(env->dump1);						\
	    break;							\
	default:							\
	    BADAGGREGATE(NAME);						\
	}								\
	env->stck = BOOLEAN_NEWNODE(result, SAVED3);			\
	POP(env->dump);							\
    }
#endif
