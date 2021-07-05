/*
    module  : __settracegc.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef __SETTRACEGC_C
#define __SETTRACEGC_C

/**
2990  __settracegc  :  I  ->
Sets value of flag for tracing garbage collection to I (= 0..2).
*/
USETOP(__settracegc_, "settracegc", NUMERICTYPE,
    tracegc = nodevalue(env->stck).num)



#endif
