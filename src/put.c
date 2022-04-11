/*
    module  : put.c
    version : 1.4
    date    : 04/11/22
*/
#ifndef PUT_C
#define PUT_C

/**
3110  put  :  X  ->
Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, my_writefactor(env, env->stck, stdout); printf(" "))



#endif
