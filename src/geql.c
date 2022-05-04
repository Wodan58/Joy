/*
    module  : geql.c
    version : 1.2
    date    : 05/04/22
*/
#ifndef GEQL_C
#define GEQL_C

#include "compare.h"

/**
2230  >=\0geql  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X greater than or equal to Y.  Also supports float.
*/
COMPREL(geql_, ">=", BOOLEAN_NEWNODE, >=, !(j & ~i))



#endif
