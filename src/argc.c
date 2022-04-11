/*
    module  : argc.c
    version : 1.2
    date    : 04/11/22
*/
#ifndef ARGC_C
#define ARGC_C

/**
3070  argc  :  ->  I
Pushes the number of command line arguments. This is quivalent to 'argv size'.
*/
PUSH(argc_, INTEGER_NEWNODE, env->g_argc)



#endif
