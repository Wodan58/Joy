/*
    module  : srand.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef SRAND_C
#define SRAND_C

/**
OK 1780  srand  :  I  ->
Sets the random integer seed to integer I.
*/
USETOP(srand_, "srand", INTEGER, srand((unsigned)nodevalue(env->stck).num))



#endif
