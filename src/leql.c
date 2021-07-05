/*
    module  : leql.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef LEQL_C
#define LEQL_C

#include "compare.h"

/**
2250  <=  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than or equal to Y.  Also supports float.
*/
COMPREL(leql_, "<=", BOOLEAN_NEWNODE, <=, !(i & ~j))



#endif
