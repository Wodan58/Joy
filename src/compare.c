/*
    module  : compare.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef COMPARE_C
#define COMPARE_C

#include "compare.h"

/**
Q0  OK  2050  compare  :  A B  ->  I
I (=-1,0,+1) is the comparison of aggregates A and B.
The values correspond to the predicates <=, =, >=.
*/
COMPREL2(compare_, "compare", INTEGER_NEWNODE, +)


#endif
