/*
    module  : time.c
    version : 1.3
    date    : 05/17/22
*/
#ifndef TIME_C
#define TIME_C

/**
1140  time  :  ->  I
Pushes the current time (in seconds since the Epoch).
*/
PUSH(time_, INTEGER_NEWNODE, (long)time(0))



#endif
