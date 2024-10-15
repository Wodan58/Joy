/*
    module  : argc.c
    version : 1.6
    date    : 09/26/24
*/
#ifndef ARGC_C
#define ARGC_C

/**
Q0  OK  3050  argc  :  ->  I
[RUNTIME] Pushes the number of command line arguments.
This is quivalent to 'argv size'.
*/
PUSH(argc_, INTEGER_NEWNODE, env->g_argc)



#endif
