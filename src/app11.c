/*
    module  : app11.c
    version : 1.6
    date    : 03/21/24
*/
#ifndef APP11_C
#define APP11_C

/**
OK 2440  app11  :  X Y [P]  ->  R
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
