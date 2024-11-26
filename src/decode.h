/*
    module  : decode.h
    version : 1.6
    date    : 11/11/24
*/
#ifndef DECODE_H
#define DECODE_H

static void decode_time(pEnv env, struct tm *t)
{
    Index p;

    t->tm_year = t->tm_mon = t->tm_mday = t->tm_hour = t->tm_min = t->tm_sec
	= t->tm_isdst = t->tm_yday = t->tm_wday = 0;
    p = nodevalue(env->stck).lis;
    if (p && nodetype(p) == INTEGER_) {
	t->tm_year = nodevalue(p).num - 1900;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_mon = nodevalue(p).num - 1;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_mday = nodevalue(p).num;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_hour = nodevalue(p).num;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_min = nodevalue(p).num;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_sec = nodevalue(p).num;
	POP(p);
    }
    if (p && nodetype(p) == BOOLEAN_) {
	t->tm_isdst = nodevalue(p).num;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_yday = nodevalue(p).num;
	POP(p);
    }
    if (p && nodetype(p) == INTEGER_) {
	t->tm_wday = nodevalue(p).num % 7;
	POP(p);
    }
}
#endif
