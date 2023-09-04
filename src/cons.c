/*
    module  : cons.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef CONS_C
#define CONS_C

/**
OK 2010  cons  :  X A  ->  B
Aggregate B is A with a new member X (first member for sequences).
*/
CONS_SWONS(cons_, "cons", env->stck, nextnode1(env->stck))



#endif
