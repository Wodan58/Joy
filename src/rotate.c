/*
    module  : rotate.c
    version : 1.3
    date    : 03/21/24
*/
#ifndef ROTATE_C
#define ROTATE_C

/**
OK 1250  rotate  :  X Y Z  ->  Z Y X
Interchanges X and Z.
*/
void rotate_(pEnv env)
{
    THREEPARAMS("rotate");
    SAVESTACK;
    GTERNARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    GNULLARY(nodetype(SAVED3), nodevalue(SAVED3));
    POP(env->dump);
}
#endif
