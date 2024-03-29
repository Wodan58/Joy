/*
    module  : nullary.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef NULLARY_C
#define NULLARY_C

/**
OK 2480  nullary  :  [P]  ->  R
Executes P, which leaves R on top of the stack.
No matter how many parameters this consumes, none are removed from the stack.
*/
N_ARY(nullary_, "nullary", ONEPARAM, SAVED2)


#endif
