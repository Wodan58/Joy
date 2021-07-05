/*
    module  : compare.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef COMPARE_C
#define COMPARE_C

#include "compare.h"

/**
2060  compare  :  A B  ->  I
I (=-1,0,+1) is the comparison of aggregates A and B.
The values correspond to the predicates <=, =, >=.
*/
COMPREL(compare_, "compare", INTEGER_NEWNODE, +, i - j < 0 ? -1 : i - j > 0)



#endif
