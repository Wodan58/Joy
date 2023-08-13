/*
    module  : binary.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef BINARY_C
#define BINARY_C

/**
OK 2580  binary  :  X Y [P]  ->  R
Executes P, which leaves R on top of the stack.
No matter how many parameters this consumes,
exactly two are removed from the stack.
*/
N_ARY(binary_, "binary", THREEPARAMS, SAVED4)

#endif
