/*
    module  : less.c
    version : 1.2
    date    : 05/04/22
*/
#ifndef LESS_C
#define LESS_C

#include "compare.h"

/**
2260  <\0less  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than Y.  Also supports float.
*/
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))



#endif
