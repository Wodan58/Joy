/*
    module  : setsize.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef SETSIZE_C
#define SETSIZE_C

/**
Q0  OK  1030  setsize  :  ->  setsize
Pushes the maximum number of elements in a set (platform dependent).
Typically it is 32, and set members are in the range 0..31.
*/
PUSH(setsize_, INTEGER_NEWNODE, SETSIZE)


#endif
