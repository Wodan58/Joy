/*
    module  : __settracegc.c
    version : 1.10
    date    : 06/21/24
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
    env->tracegc = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
