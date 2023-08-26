/*
    module  : round.c
    version : 1.1
    date    : 08/26/23
*/
#ifndef ROUND_C
#define ROUND_C

/**
OK 1693  round  :  F  ->  G
G is F rounded to the nearest integer.
*/
double round2(double num)
{
    if (num < 0)
	return -floor(-num + 0.5);
    return floor(num + 0.5);
}

void round_(pEnv env)
{
    double num;

    ONEPARAM("round");
    FLOAT("round");
    if (nodetype(env->stck) == FLOAT_)
	num = nodevalue(env->stck).dbl;
    else
	num = nodevalue(env->stck).num;
    UNARY(FLOAT_NEWNODE, round2(num));
}
#endif
