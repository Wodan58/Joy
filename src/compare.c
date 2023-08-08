/*
    module  : compare.c
    version : 1.2
    date    : 08/08/23
*/
#ifndef COMPARE_C
#define COMPARE_C

#include "compare.h"

/**
2060  compare  :  A B  ->  I
I (=-1,0,+1) is the comparison of aggregates A and B.
The values correspond to the predicates <=, =, >=.
*/
COMPREL(compare_, "compare", INTEGER_NEWNODE, +, i < j ? -1 : i - j > 0)



#endif
