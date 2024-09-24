/*
    module  : put.c
    version : 1.10
    date    : 09/17/24
*/
#ifndef PUT_C
#define PUT_C

/**
Q0  IGNORE_POP  3080  put  :  X  ->
[IMPURE] Writes X to output, pops X off stack.
*/
USETOP(put_, "put", ONEPARAM, writefactor(env, env->stck, stdout))



#endif
