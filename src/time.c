/*
    module  : time.c
    version : 1.5
    date    : 08/13/23
*/
#ifndef TIME_C
#define TIME_C

/**
OK 1140  time  :  ->  I
Pushes the current time (in seconds since the Epoch).
*/
PUSH(time_, INTEGER_NEWNODE, (int64_t)time(0))



#endif
