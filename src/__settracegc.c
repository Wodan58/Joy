/*
    module  : __settracegc.c
    version : 1.2
    date    : 04/11/22
*/
#ifndef __SETTRACEGC_C
#define __SETTRACEGC_C

/**
2990  __settracegc  :  I  ->
Sets value of flag for tracing garbage collection to I (= 0..6).
*/
USETOP(__settracegc_, "settracegc", NUMERICTYPE,
    env->tracegc = nodevalue(env->stck).num)



#endif
