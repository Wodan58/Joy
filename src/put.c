/*
    module  : put.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef PUT_C
#define PUT_C

/**
3110  put  :  X  ->
Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout); printf(" "))



#endif
