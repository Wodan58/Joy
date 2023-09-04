/*
    module  : __settracegc.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef __SETTRACEGC_C
#define __SETTRACEGC_C

/**
OK 2970  __settracegc  :  I  ->
Sets value of flag for tracing garbage collection to I (= 0..6).
*/
USETOP(__settracegc_, "settracegc", NUMERICTYPE,
    env->tracegc = nodevalue(env->stck).num)


#endif
