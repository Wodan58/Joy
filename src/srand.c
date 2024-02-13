/*
    module  : srand.c
    version : 1.5
    date    : 01/22/24
*/
#ifndef SRAND_C
#define SRAND_C

/**
OK 1780  srand  :  I  ->
[IMPURE] Sets the random integer seed to integer I.
*/
USETOP(srand_, "srand", INTEGER, srand((unsigned)nodevalue(env->stck).num))



#endif
