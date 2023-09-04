/*
    module  : get.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef GET_C
#define GET_C

/**
OK 3070  get  :  ->  F
Reads a factor from input and pushes it onto stack.
*/
PRIVATE void get_(pEnv env)
{
    getsym(env);
    readfactor(env);
}
#endif
