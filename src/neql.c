/*
    module  : neql.c
    version : 1.5
    date    : 04/11/24
*/
#ifndef NEQL_C
#define NEQL_C

#include "compare.h"

/**
OK 2260  !=\0neql  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X not equal to Y.  Also supports float.
*/
COMPREL2(neql_, "!=", BOOLEAN_NEWNODE, !=)


#endif
