/*
    module  : put.c
    version : 1.9
    date    : 04/11/24
*/
#ifndef PUT_C
#define PUT_C

/**
IGNORE_POP  3080  put  :  X  ->
[IMPURE] Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout))



#endif
