/*
    module  : cons.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef CONS_C
#define CONS_C

/**
OK 2020  cons  :  X A  ->  B
Aggregate B is A with a new member X (first member for sequences).
*/
CONS_SWONS(cons_, "cons", env->stck, nextnode1(env->stck))



#endif
