/*
    module  : get.c
    version : 1.3
    date    : 08/23/23
*/
#ifndef GET_C
#define GET_C

/**
OK 3090  get  :  ->  F
Reads a factor from input and pushes it onto stack.
*/
PRIVATE void get_(pEnv env)
{
    getsym(env);
    readfactor(env);
}
#endif
