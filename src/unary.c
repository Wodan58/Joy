/*
    module  : unary.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef UNARY_C
#define UNARY_C

/**
Q1  OK  2490  unary  :  X [P]  ->  R
Executes P, which leaves R on top of the stack.
No matter how many parameters this consumes,
exactly one is removed from the stack.
*/
N_ARY(unary_, "unary", TWOPARAMS, SAVED3)

#endif
