/*
    module  : rolldown.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef ROLLDOWN_C
#define ROLLDOWN_C

/**
1240  rolldown  :  X Y Z  ->  Y Z X
Moves Y and Z down, moves X up.
*/
PRIVATE void rolldown_(pEnv env)
{
    THREEPARAMS("rolldown");
    SAVESTACK;
    GTERNARY(nodetype(SAVED2), nodevalue(SAVED2));
    GNULLARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    POP(env->dump);
}
#endif
