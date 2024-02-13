/*
    module  : time.c
    version : 1.7
    date    : 01/22/24
*/
#ifndef TIME_C
#define TIME_C

/**
OK 1140  time  :  ->  I
[IMPURE] Pushes the current time (in seconds since the Epoch).
*/
PUSH2(time_, INTEGER_NEWNODE, (int64_t)time(0))



#endif
