/*
    module  : setsize.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef SETSIZE_C
#define SETSIZE_C

/**
OK  1030  setsize  :  ->  setsize
Pushes the maximum number of elements in a set (platform dependent).
Typically it is 32, and set members are in the range 0..31.
*/
PUSH(setsize_, INTEGER_NEWNODE, SETSIZE)


#endif
