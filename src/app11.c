/*
    module  : app11.c
    version : 1.4
    date    : 08/13/23
*/
#ifndef APP11_C
#define APP11_C

/**
OK 2470  app11  :  X Y [P]  ->  R
Executes P, pushes result R on stack.
*/
PRIVATE void app11_(pEnv env)
{
    THREEPARAMS("app11");
    ONEQUOTE("app11");
    app1_(env);
    if (nextnode1(env->stck))
	nextnode1(env->stck) = nextnode2(env->stck);
}
#endif
