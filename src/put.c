/*
    module  : put.c
    version : 1.2
    date    : 05/02/22
*/
#ifndef PUT_C
#define PUT_C

/**
3110  put  :  X  ->
Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck); printf(" "))



#endif
