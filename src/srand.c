/*
    module  : srand.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef SRAND_C
#define SRAND_C

void my_srand(unsigned num);

/**
1780  srand  :  I  ->
Sets the random integer seed to integer I.
*/
USETOP(srand_, "srand", INTEGER, my_srand((unsigned)nodevalue(env->stck).num))



#endif
