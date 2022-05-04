/*
    module  : greater.c
    version : 1.2
    date    : 05/04/22
*/
#ifndef GREATER_C
#define GREATER_C

#include "compare.h"

/**
2240  >\0greater  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X greater than Y.  Also supports float.
*/
COMPREL(greater_, ">", BOOLEAN_NEWNODE, >, i != j && !(j & ~i))



#endif
