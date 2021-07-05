/*
    module  : ternary.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef TERNARY_C
#define TERNARY_C

/**
2590  ternary  :  X Y Z [P]  ->  R
Executes P, which leaves R on top of the stack.
No matter how many parameters this consumes,
exactly three are removed from the stack.
*/
N_ARY(ternary_, "ternary", FOURPARAMS, SAVED5)



#endif
