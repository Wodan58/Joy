/*
    module  : time.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef TIME_C
#define TIME_C

/**
Q0  IGNORE_PUSH  1140  time  :  ->  I
[IMPURE] Pushes the current time (in seconds since the Epoch).
*/
PUSH(time_, INTEGER_NEWNODE, (int64_t)time(0))



#endif
