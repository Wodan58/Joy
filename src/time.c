/*
    module  : time.c
    version : 1.2
    date    : 07/20/21
*/
#ifndef TIME_C
#define TIME_C

/**
1140  time  :  ->  I
Pushes the current time (in seconds since the Epoch).
*/
PUSH(time_, INTEGER_NEWNODE, (long_t)time(NULL))



#endif
