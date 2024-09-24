/*
    module  : rotate.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef ROTATE_C
#define ROTATE_C

/**
Q0  OK  1250  rotate  :  X Y Z  ->  Z Y X
Interchanges X and Z.
*/
void rotate_(pEnv env)
{
    THREEPARAMS("rotate");
    SAVESTACK;
    GTERNARY(SAVED1);
    GNULLARY(SAVED2);
    GNULLARY(SAVED3);
    POP(env->dump);
}
#endif
