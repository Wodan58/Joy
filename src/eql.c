/*
    module  : eql.c
    version : 1.5
    date    : 04/11/24
*/
#ifndef EQL_C
#define EQL_C

#include "compare.h"

/**
OK 2270  =\0equals  :  X Y  ->  B
Either both X and Y are numeric or both are strings or symbols.
Tests whether X equal to Y.  Also supports float.
*/
COMPREL2(eql_, "=", BOOLEAN_NEWNODE, ==)


#endif
