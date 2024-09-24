/*
    module  : cons.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef CONS_C
#define CONS_C

/**
Q0  OK  2010  cons  :  X A  ->  B
Aggregate B is A with a new member X (first member for sequences).
*/
CONS_SWONS(cons_, "cons", env->stck, nextnode1(env->stck))



#endif
