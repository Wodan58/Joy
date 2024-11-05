/*
    module  : argc.c
    version : 1.7
    date    : 10/24/24
*/
#ifndef ARGC_C
#define ARGC_C

/**
Q0  OK  3050  argc  :  ->  I
[RUNTIME] Pushes the number of command line arguments.
This is equivalent to 'argv size'.
*/
PUSH(argc_, INTEGER_NEWNODE, env->g_argc)



#endif
