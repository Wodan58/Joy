/*
    module  : rolldown.c
    version : 1.4
    date    : 06/21/24
*/
#ifndef ROLLDOWN_C
#define ROLLDOWN_C

/**
OK 1240  rolldown  :  X Y Z  ->  Y Z X
Moves Y and Z down, moves X up.
*/
void rolldown_(pEnv env)
{
    THREEPARAMS("rolldown");
    SAVESTACK;
    GTERNARY(SAVED2);
    GNULLARY(SAVED1);
    GNULLARY(SAVED3);
    POP(env->dump);
}
#endif
