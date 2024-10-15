/*
    module  : __settracegc.c
    version : 1.12
    date    : 10/11/24
*/
#ifndef __SETTRACEGC_C
#define __SETTRACEGC_C

/**
Q0  OK  2970  __settracegc  :  I  ->
[IMPURE] Sets value of flag for tracing garbage collection to I (= 0..6).
*/
void __settracegc_(pEnv env)
{
    ONEPARAM("settracegc");
    NUMERICTYPE("settracegc");
    env->tracegc = nodevalue(env->stck).num;
#ifndef NOBDW
    if (env->tracegc)		/* 0=enable bytecoding or compiling */
	;
    /*
     * The flags are initially negative; when activating, they are made
     * positive.
     */
    else if (env->bytecoding)
	env->bytecoding = -env->bytecoding;	/* LCOV_EXCL_LINE */
    else if (env->compiling)
	env->compiling = -env->compiling;	/* LCOV_EXCL_LINE */
    else
	env->ignore = 0;	/* disable ignore */
#endif
    POP(env->stck);
}
#endif
