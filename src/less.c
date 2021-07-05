/*
    module  : less.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef LESS_C
#define LESS_C

#include "compare.h"

/**
2260  <  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than Y.  Also supports float.
*/
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))



#endif
