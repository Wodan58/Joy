/*
    module  : clock.c
    version : 1.4
    date    : 08/18/23
*/
#ifndef CLOCK_C
#define CLOCK_C

/**
OK 1130  clock  :  ->  I
Pushes the integer value of current CPU usage in milliseconds.
*/
PUSH(clock_, INTEGER_NEWNODE, ((clock() - env->startclock) * 1000 / CLOCKS_PER_SEC)



#endif
