/*
    module  : put.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef PUT_C
#define PUT_C

/**
IGNORE_POP  3080  put  :  X  ->
[IMPURE] Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout); printf(" "))



#endif
