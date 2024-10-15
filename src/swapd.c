/*
    module  : swapd.c
    version : 1.4
    date    : 10/11/24
*/
#ifndef SWAPD_C
#define SWAPD_C

#include "swap.c"

/**
Q0  OK  1280  swapd  :  X Y Z  ->  Y X Z
As if defined by:   swapd  ==  [swap] dip
*/
DIPPED(swapd_, "swapd", THREEPARAMS, swap_)



#endif
