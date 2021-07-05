/*
    module  : swap.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef SWAP_C
#define SWAP_C

/**
1220  swap  :  X Y  ->  Y X
Interchanges X and Y on top of the stack.
*/
PRIVATE void swap_(pEnv env)
{
    TWOPARAMS("swap");
    SAVESTACK;
    GBINARY(nodetype(SAVED1), nodevalue(SAVED1));
    GNULLARY(nodetype(SAVED2), nodevalue(SAVED2));
    POP(env->dump);
}
#endif
