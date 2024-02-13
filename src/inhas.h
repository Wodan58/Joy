/*
    module  : inhas.h
    version : 1.6
    date    : 02/01/24
*/
#ifndef INHAS_H
#define INHAS_H

#define INHAS(PROCEDURE, NAME, AGGR, ELEM)				\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	int found = 0;							\
	char *str;							\
	Index node;							\
	TWOPARAMS(NAME);						\
	switch (nodetype(AGGR)) {					\
	case SET_:							\
	    CHECKSETMEMBER(ELEM, NAME);					\
	    found = ((nodevalue(AGGR).set) &				\
		    ((int64_t)1 << nodevalue(ELEM).num)) > 0;		\
	    break;							\
	case STRING_:							\
	    for (str = nodevalue(AGGR).str;				\
		 *str != '\0' && *str != nodevalue(ELEM).num; str++)	\
		;							\
	    found = *str != '\0';					\
	    break;							\
	case LIST_:							\
	    node = nodevalue(AGGR).lis;					\
	    while (node && Compare(env, node, ELEM))			\
		node = nextnode1(node);					\
	    found = node != 0;						\
	    break;							\
	default:							\
	    BADAGGREGATE(NAME);						\
	}								\
	BINARY(BOOLEAN_NEWNODE, found);					\
    }
#endif
