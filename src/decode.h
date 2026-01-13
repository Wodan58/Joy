/*
    module  : decode.h
    version : 1.7
    date    : 01/12/26
*/
#ifndef DECODE_H
#define DECODE_H

static void decode_time(pEnv env, struct tm *t)
{
    Index p;

    memset(t, 0, sizeof(struct tm));
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
