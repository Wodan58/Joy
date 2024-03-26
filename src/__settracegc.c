/*
    module  : __settracegc.c
    version : 1.9
    date    : 03/21/24
*/
#ifndef __SETTRACEGC_C
#define __SETTRACEGC_C

/**
OK 2970  __settracegc  :  I  ->
[IMPURE] Sets value of flag for tracing garbage collection to I (= 0..6).
*/
void __settracegc_(pEnv env)
{
    ONEPARAM("settracegc");
    NUMERICTYPE("settracegc");
    if (!env->ignore)
	env->tracegc = nodevalue(env->stck).num;
    else if (!nodevalue(env->stck).num)
	env->ignore = 0;
    POP(env->stck);
}
#endif
