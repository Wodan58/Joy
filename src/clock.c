/*
    module  : clock.c
    version : 1.7
    date    : 01/22/24
*/
#ifndef CLOCK_C
#define CLOCK_C

/**
OK 1130  clock  :  ->  I
[IMPURE] Pushes the integer value of current CPU usage in milliseconds.
*/
PUSH2(clock_, INTEGER_NEWNODE,
	((clock() - env->startclock) * 1000 / CLOCKS_PER_SEC))


#endif
