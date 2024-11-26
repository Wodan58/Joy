/*
    module  : someall.h
    version : 1.10
    date    : 11/06/24
*/
#ifndef SOMEALL_H
#define SOMEALL_H

#define SOMEALL(PROCEDURE, NAME, INITIAL)				\
    void PROCEDURE(pEnv env)						\
    {									\
	int i = 0, result, end_result = INITIAL;			\
	char *str;							\
	TWOPARAMS(NAME);						\
	ONEQUOTE(NAME);							\
	SAVESTACK;							\
	switch (nodetype(SAVED2)) {					\
	case SET_:							\
	    for (; i < SETSIZE; i++)					\
		if (nodevalue(SAVED2).set & ((int64_t)1 << i)) {	\
		    env->stck = INTEGER_NEWNODE(i, SAVED3);		\
		    exeterm(env, nodevalue(SAVED1).lis);		\
		    CHECKSTACK(NAME);					\
		    result = get_boolean(env, env->stck);		\
		    if (result != INITIAL) {				\
			end_result = 1 - INITIAL;			\
			break;						\
		    }							\
		}							\
	    break;							\
	case STRING_:							\
	    for (str = strdup((char *)&nodevalue(SAVED2)); str[i]; i++) {\
		env->stck = CHAR_NEWNODE(str[i], SAVED3);		\
		exeterm(env, nodevalue(SAVED1).lis);			\
		CHECKSTACK(NAME);					\
		result = get_boolean(env, env->stck);			\
		if (result != INITIAL) {				\
		    end_result = 1 - INITIAL;				\
		    break;						\
		}							\
	    }								\
	    free(str);							\
	    break;							\
	case LIST_:							\
	    env->dump1 = LIST_NEWNODE(nodevalue(SAVED2).lis, env->dump1);\
	    for (; DMP1; DMP1 = nextnode1(DMP1)) {			\
		env->stck = newnode2(env, DMP1, SAVED3);		\
		exeterm(env, nodevalue(SAVED1).lis);			\
		CHECKSTACK(NAME);					\
		result = get_boolean(env, env->stck);			\
		if (result != INITIAL) {				\
		    end_result = 1 - INITIAL;				\
		    break;						\
		}							\
	    }								\
	    POP(env->dump1);						\
	    break;							\
	default:							\
	    BADAGGREGATE(NAME);						\
	}								\
	env->stck = BOOLEAN_NEWNODE(end_result, SAVED3);		\
	POP(env->dump);							\
    }
#endif
