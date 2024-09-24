/*
    module  : nullary.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef NULLARY_C
#define NULLARY_C

/**
Q1  OK  2480  nullary  :  [P]  ->  R
Executes P, which leaves R on top of the stack.
No matter how many parameters this consumes, none are removed from the stack.
*/
N_ARY(nullary_, "nullary", ONEPARAM, SAVED2)


#endif
