/*
    module  : unary.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef UNARY_C
#define UNARY_C

/**
2510  unary  :  X [P]  ->  R
Executes P, which leaves R on top of the stack.
No matter how many parameters this consumes,
exactly one is removed from the stack.
*/
N_ARY(unary_, "unary", TWOPARAMS, SAVED3)



#endif
