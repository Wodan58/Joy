/*
    module  : of_at.h
    version : 1.11
    date    : 09/17/24
*/
#ifndef OF_AT_H
#define OF_AT_H

#define OF_AT(PROCEDURE, NAME, AGGR, INDEX)				\
    void PROCEDURE(pEnv env)						\
    {									\
	Index n;							\
	char *str;							\
	int i, indx;							\
	TWOPARAMS(NAME);						\
	POSITIVEINDEX(INDEX, NAME);					\
	indx = nodevalue(INDEX).num;					\
	switch (nodetype(AGGR)) {					\
	case SET_:							\
	    CHECKEMPTYSET(nodevalue(AGGR).set, NAME);			\
	    for (i = 0; i < SETSIZE; i++) {				\
		if (nodevalue(AGGR).set & ((int64_t)1 << i)) {		\
		    if (indx == 0) {					\
			BINARY(INTEGER_NEWNODE, i);			\
			return;						\
		    }							\
		    indx--;						\
		}							\
	    }								\
	    INDEXTOOLARGE(NAME);					\
	    break;							\
	case STRING_:							\
	    if (indx >= (int)nodeleng(AGGR))				\
		INDEXTOOLARGE(NAME);					\
	    str = (char *)&nodevalue(AGGR);				\
	    BINARY(CHAR_NEWNODE, str[indx]);				\
	    break;							\
	case LIST_:							\
	    n = nodevalue(AGGR).lis;					\
	    CHECKEMPTYLIST(n, NAME);					\
	    while (indx > 0) {						\
		if (!nextnode1(n))					\
		    INDEXTOOLARGE(NAME);				\
		n = nextnode1(n);					\
		indx--;							\
	    }								\
	    GBINARY(n);							\
	    break;							\
	default:							\
	    BADAGGREGATE(NAME);						\
	}								\
    }
#endif
