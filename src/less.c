/*
    module  : less.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef LESS_C
#define LESS_C

#include "compare.h"

/**
OK 2260  <\0less  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than Y.  Also supports float.
*/
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))


#endif
