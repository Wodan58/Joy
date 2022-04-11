/*
    module  : srand.c
    version : 1.2
    date    : 04/11/22
*/
#ifndef SRAND_C
#define SRAND_C

/**
1780  srand  :  I  ->
Sets the random integer seed to integer I.
*/
USETOP(srand_, "srand", INTEGER, srand((unsigned)nodevalue(env->stck).num))



#endif
