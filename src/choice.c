/*
    module  : choice.c
    version : 1.4
    date    : 06/21/24
*/
#ifndef CHOICE_C
#define CHOICE_C

/**
OK 1330  choice  :  B T F  ->  X
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
