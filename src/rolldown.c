/*
    module  : rolldown.c
    version : 1.3
    date    : 03/21/24
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
    GTERNARY(nodetype(SAVED2), nodevalue(SAVED2));
    GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    POP(env->dump);
}
#endif
