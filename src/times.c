/*
    module  : times.c
    version : 1.5
    date    : 06/21/24
*/
#ifndef TIMES_C
#define TIMES_C

/**
OK 2800  times  :  N [P]  ->  ...
N times executes P.
*/
void times_(pEnv env)
{
    int i, n;

    TWOPARAMS("times");
    ONEQUOTE("times");
    INTEGER2("times");
    SAVESTACK;
    env->stck = nextnode2(env->stck);
    n = nodevalue(SAVED2).num;
    for (i = 1; i <= n; i++)
	exeterm(env, nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
