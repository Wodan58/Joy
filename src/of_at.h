/*
    module  : of_at.h
    version : 1.8
    date    : 03/21/24
*/
#ifndef OF_AT_H
#define OF_AT_H

#define OF_AT(PROCEDURE, NAME, AGGR, INDEX)				\
    void PROCEDURE(pEnv env)						\
    {									\
	Index n;							\
	int i, indx;							\
	TWOPARAMS(NAME);						\
	POSITIVEINDEX(INDEX, NAME);					\
	switch (nodetype(AGGR)) {					\
	case SET_:							\
	    indx = nodevalue(INDEX).num;				\
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
	    if (strlen(nodevalue(AGGR).str) < (size_t)nodevalue(INDEX).num) { \
		INDEXTOOLARGE(NAME); }					\
	    BINARY(CHAR_NEWNODE, nodevalue(AGGR).str[nodevalue(INDEX).num]); \
	    break;							\
	case LIST_:							\
	    n = nodevalue(AGGR).lis;					\
	    i = nodevalue(INDEX).num;					\
	    CHECKEMPTYLIST(n, NAME);					\
	    while (i > 0) {						\
		if (!nextnode1(n)) {					\
		    INDEXTOOLARGE(NAME); }				\
		n = nextnode1(n);					\
		i--;							\
	    }								\
	    GBINARY(nodetype(n), nodevalue(n));				\
	    break;							\
	default:							\
	    BADAGGREGATE(NAME);						\
	}								\
    }
#endif
