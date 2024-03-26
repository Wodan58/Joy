/*
    module  : time.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef TIME_C
#define TIME_C

/**
IGNORE_PUSH  1140  time  :  ->  I
[IMPURE] Pushes the current time (in seconds since the Epoch).
*/
PUSH(time_, INTEGER_NEWNODE, (int64_t)time(0))



#endif
