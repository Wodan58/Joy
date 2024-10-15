/*
    module  : rolldownd.c
    version : 1.4
    date    : 10/11/24
*/
#ifndef ROLLDOWND_C
#define ROLLDOWND_C

#include "rolldown.c"

/**
Q0  OK  1300  rolldownd  :  X Y Z W  ->  Y Z X W
As if defined by:   rolldownd  ==  [rolldown] dip
*/
DIPPED(rolldownd_, "rolldownd", FOURPARAMS, rolldown_)



#endif
