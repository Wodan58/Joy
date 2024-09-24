/*
    module  : app11.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef APP11_C
#define APP11_C

/**
Q1  OK  2440  app11  :  X Y [P]  ->  R
Executes P, pushes result R on stack.
*/
void app11_(pEnv env)
{
    THREEPARAMS("app11");
    ONEQUOTE("app11");
    i_(env);
    if (nextnode1(env->stck))
	nextnode1(env->stck) = nextnode2(env->stck);
}
#endif
