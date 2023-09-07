/*
    module  : put.c
    version : 1.5
    date    : 09/07/23
*/
#ifndef PUT_C
#define PUT_C

/**
OK 3080  put  :  X  ->
Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout); printf(" "))



#endif
