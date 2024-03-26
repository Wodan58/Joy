/*
    module  : swap.c
    version : 1.3
    date    : 03/21/24
*/
#ifndef SWAP_C
#define SWAP_C

/**
OK 1220  swap  :  X Y  ->  Y X
Interchanges X and Y on top of the stack.
*/
void swap_(pEnv env)
{
    TWOPARAMS("swap");
    SAVESTACK;
    GBINARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}
#endif
