/*
    module  : cons.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef CONS_C
#define CONS_C

/**
2020  cons  :  X A  ->  B
Aggregate B is A with a new member X (first member for sequences).
*/
CONS_SWONS(cons_, "cons", env->stck, nextnode1(env->stck))



#endif
