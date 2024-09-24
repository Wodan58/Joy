/*
    module  : less.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef LESS_C
#define LESS_C

#include "compare.h"

/**
Q0  OK  2250  <\0less  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than Y.  Also supports float.
*/
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))


#endif
