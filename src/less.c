/*
    module  : less.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef LESS_C
#define LESS_C

#include "compare.h"

/**
OK 2250  <\0less  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than Y.  Also supports float.
*/
COMPREL(less_, "<", BOOLEAN_NEWNODE, <, i != j && !(i & ~j))


#endif
