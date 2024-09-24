/*
    module  : clock.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef CLOCK_C
#define CLOCK_C

/**
Q0  IGNORE_PUSH  1130  clock  :  ->  I
[IMPURE] Pushes the integer value of current CPU usage in milliseconds.
*/
PUSH(clock_, INTEGER_NEWNODE,
	((clock() - env->startclock) * 1000 / CLOCKS_PER_SEC))


#endif
