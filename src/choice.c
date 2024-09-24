/*
    module  : choice.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef CHOICE_C
#define CHOICE_C

/**
Q0  OK  1330  choice  :  B T F  ->  X
If B is true, then X = T else X = F.
*/
void choice_(pEnv env)
{
    THREEPARAMS("choice");
    if (nodevalue(nextnode2(env->stck)).num)
	GTERNARY(nextnode1(env->stck));
    else
	GTERNARY(env->stck);
}
#endif
