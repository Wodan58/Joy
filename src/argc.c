/*
    module  : argc.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef ARGC_C
#define ARGC_C

/**
OK 3070  argc  :  ->  I
Pushes the number of command line arguments. This is quivalent to 'argv size'.
*/
PUSH(argc_, INTEGER_NEWNODE, env->g_argc)



#endif
