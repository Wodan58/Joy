/*
    module  : unmktime.h
    version : 1.8
    date    : 03/22/24
*/
#ifndef UNMKTIME_H
#define UNMKTIME_H

#define UNMKTIME(PROCEDURE, NAME, FUNC)					\
    void PROCEDURE(pEnv env)						\
    {									\
	struct tm *t;							\
	int64_t wday;							\
	time_t timval;							\
	ONEPARAM(NAME);							\
	INTEGER(NAME);							\
	timval = nodevalue(env->stck).num;				\
	t = FUNC(&timval);						\
	wday = t->tm_wday;						\
	if (wday == 0)							\
	    wday = 7;							\
	env->dump1 = LIST_NEWNODE(0, env->dump1);			\
	DMP1 = INTEGER_NEWNODE(wday, DMP1);				\
	DMP1 = INTEGER_NEWNODE(t->tm_yday, DMP1);			\
	DMP1 = BOOLEAN_NEWNODE(t->tm_isdst, DMP1);			\
	DMP1 = INTEGER_NEWNODE(t->tm_sec, DMP1);			\
	DMP1 = INTEGER_NEWNODE(t->tm_min, DMP1);			\
	DMP1 = INTEGER_NEWNODE(t->tm_hour, DMP1);			\
	DMP1 = INTEGER_NEWNODE(t->tm_mday, DMP1);			\
	DMP1 = INTEGER_NEWNODE((t->tm_mon + 1), DMP1);			\
	DMP1 = INTEGER_NEWNODE((t->tm_year + 1900), DMP1);		\
	UNARY(LIST_NEWNODE, DMP1);					\
	POP(env->dump1);						\
    }
#endif
