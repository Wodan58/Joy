/*
    module  : get.c
    version : 1.8
    date    : 09/17/24
*/
#ifndef GET_C
#define GET_C

/**
Q0  POSTPONE  3070  get  :  ->  F
[IMPURE] Reads a factor from input and pushes it onto stack.
*/
void get_(pEnv env)
{
    int ch, rv;

    ch = getch(env);
    ch = getsym(env, ch);
    ch = readfactor(env, ch, &rv);
    ungetch(ch);
}
#endif
