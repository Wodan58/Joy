/*
    module  : choice.c
    version : 1.7
    date    : 11/11/24
*/
#ifndef CHOICE_C
#define CHOICE_C

#include "boolean.h"

/**
Q0  OK  1330  choice  :  B T F  ->  X
If B is true, then X = T else X = F.
*/
void choice_(pEnv env)
{
    int result;

    THREEPARAMS("choice");
    result = get_boolean(env, nextnode2(env->stck));
    if (result)
	GTERNARY(nextnode1(env->stck));
    else
	GTERNARY(env->stck);
}
#endif
