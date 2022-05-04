/*
    module  : eql.c
    version : 1.2
    date    : 05/04/22
*/
#ifndef EQL_C
#define EQL_C

#include "compare.h"

/**
2280  =\0equals  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X equal to Y.  Also supports float.
*/
COMPREL(eql_, "=", BOOLEAN_NEWNODE, ==, i == j)



#endif
