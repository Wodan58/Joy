/*
    module  : leql.c
    version : 1.2
    date    : 05/04/22
*/
#ifndef LEQL_C
#define LEQL_C

#include "compare.h"

/**
2250  <=\0leql  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X less than or equal to Y.  Also supports float.
*/
COMPREL(leql_, "<=", BOOLEAN_NEWNODE, <=, !(i & ~j))



#endif
