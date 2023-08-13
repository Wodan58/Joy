/*
    module  : get.c
    version : 1.2
    date    : 08/13/23
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
    readfactor(env, 0);
}
#endif
