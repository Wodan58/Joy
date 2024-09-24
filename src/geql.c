/*
    module  : geql.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef GEQL_C
#define GEQL_C

#include "compare.h"

/**
Q0  OK  2220  >=\0geql  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X greater than or equal to Y.  Also supports float.
*/
COMPREL(geql_, ">=", BOOLEAN_NEWNODE, >=, !(j & ~i))


#endif
