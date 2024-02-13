/*
    module  : __settracegc.c
    version : 1.7
    date    : 02/01/24
*/
#ifndef __SETTRACEGC_C
#define __SETTRACEGC_C

/**
OK 2970  __settracegc  :  I  ->
[IMPURE] Sets value of flag for tracing garbage collection to I (= 0..6).
*/
PRIVATE void __settracegc_(pEnv env)
{
    ONEPARAM("settracegc");
    NUMERICTYPE("settracegc");
    if (!env->ignore && !env->tracegc_set)
	env->tracegc = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
