/*
    module  : eql.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef EQL_C
#define EQL_C

#include "compare.h"

/**
Q0  OK  2270  =\0eql  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X equal to Y.  Also supports float.
*/
COMPREL2(eql_, "=", BOOLEAN_NEWNODE, ==)


#endif
