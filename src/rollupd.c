/*
    module  : rollupd.c
    version : 1.4
    date    : 10/11/24
*/
#ifndef ROLLUPD_C
#define ROLLUPD_C

#include "rollup.c"

/**
Q0  OK  1290  rollupd  :  X Y Z W  ->  Z X Y W
As if defined by:   rollupd  ==  [rollup] dip
*/
DIPPED(rollupd_, "rollupd", FOURPARAMS, rollup_)



#endif
