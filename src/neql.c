/*
    module  : neql.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef NEQL_C
#define NEQL_C

#include "compare.h"

/**
2270  !=  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X not equal to Y.  Also supports float.
*/
COMPREL(neql_, "!=", BOOLEAN_NEWNODE, !=, i != j)



#endif
