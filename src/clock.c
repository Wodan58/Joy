/*
    module  : clock.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef CLOCK_C
#define CLOCK_C

/**
OK 1130  clock  :  ->  I
Pushes the integer value of current CPU usage in milliseconds.
*/
PUSH(clock_, INTEGER_NEWNODE, (clock() - env->startclock))



#endif
