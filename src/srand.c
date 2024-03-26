/*
    module  : srand.c
    version : 1.6
    date    : 03/21/24
*/
#ifndef SRAND_C
#define SRAND_C

/**
IGNORE_POP  1780  srand  :  I  ->
[IMPURE] Sets the random integer seed to integer I.
*/
USETOP(srand_, "srand", INTEGER, srand((unsigned)nodevalue(env->stck).num))



#endif
