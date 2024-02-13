/*
    module  : put.c
    version : 1.7
    date    : 01/22/24
*/
#ifndef PUT_C
#define PUT_C

/**
OK 3080  put  :  X  ->
[IMPURE] Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout); printf(" "))



#endif
