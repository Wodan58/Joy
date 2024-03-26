/*
    module  : clock.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef CLOCK_C
#define CLOCK_C

/**
IGNORE_PUSH  1130  clock  :  ->  I
[IMPURE] Pushes the integer value of current CPU usage in milliseconds.
*/
PUSH(clock_, INTEGER_NEWNODE,
	((clock() - env->startclock) * 1000 / CLOCKS_PER_SEC))


#endif
