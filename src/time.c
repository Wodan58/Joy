/*
    module  : time.c
    version : 1.4
    date    : 07/19/23
*/
#ifndef TIME_C
#define TIME_C

/**
1140  time  :  ->  I
Pushes the current time (in seconds since the Epoch).
*/
PUSH(time_, INTEGER_NEWNODE, (int64_t)time(0))



#endif
