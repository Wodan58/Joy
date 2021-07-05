/*
    module  : rollup.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef ROLLUP_C
#define ROLLUP_C

/**
1230  rollup  :  X Y Z  ->  Z X Y
Moves X and Y up, moves Z down.
*/
PRIVATE void rollup_(pEnv env)
{
    THREEPARAMS("rollup");
    SAVESTACK;
    GTERNARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}
#endif
