/*
    module  : rollup.c
    version : 1.4
    date    : 06/21/24
*/
#ifndef ROLLUP_C
#define ROLLUP_C

/**
OK 1230  rollup  :  X Y Z  ->  Z X Y
Moves X and Y up, moves Z down.
*/
void rollup_(pEnv env)
{
    THREEPARAMS("rollup");
    SAVESTACK;
    GTERNARY(SAVED1);
    GNULLARY(SAVED3);
    GNULLARY(SAVED2);
    POP(env->dump);
}
#endif
