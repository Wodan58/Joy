/*
    module  : get.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef GET_C
#define GET_C

/**
3090  get  :  ->  F
Reads a factor from input and pushes it onto stack.
*/
PRIVATE void get_(pEnv env)
{
    getsym(env);
    readfactor(env, 0);
}
#endif
