/*
    module  : geql.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef GEQL_C
#define GEQL_C

#include "compare.h"

/**
OK 2220  >=\0geql  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X greater than or equal to Y.  Also supports float.
*/
COMPREL(geql_, ">=", BOOLEAN_NEWNODE, >=, !(j & ~i))


#endif
