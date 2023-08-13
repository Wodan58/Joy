/*
    module  : put.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef PUT_C
#define PUT_C

/**
OK 3110  put  :  X  ->
Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck); printf(" "))



#endif
