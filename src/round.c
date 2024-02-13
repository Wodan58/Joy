/*
    module  : round.c
    version : 1.3
    date    : 02/01/24
*/
#ifndef ROUND_C
#define ROUND_C

/**
OK 3200  round  :  F  ->  G
[EXT] G is F rounded to the nearest integer.
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
