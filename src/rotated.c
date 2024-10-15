/*
    module  : rotated.c
    version : 1.5
    date    : 10/11/24
*/
#ifndef ROTATED_C
#define ROTATED_C

#include "rotate.c"

/**
Q0  OK  1310  rotated  :  X Y Z W  ->  Z Y X W
As if defined by:   rotated  ==  [rotate] dip
*/
DIPPED(rotated_, "rotated", FOURPARAMS, rotate_)



#endif
