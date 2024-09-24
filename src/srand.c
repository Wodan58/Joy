/*
    module  : srand.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef SRAND_C
#define SRAND_C

/**
Q0  IGNORE_POP  1780  srand  :  I  ->
[IMPURE] Sets the random integer seed to integer I.
*/
USETOP(srand_, "srand", INTEGER, srand((unsigned)nodevalue(env->stck).num))



#endif
