/*
    module  : getch.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef GETCH_C
#define GETCH_C

/**
POSTPONE  3160  getch  :  ->  N
[IMPURE] Reads a character from input and puts it onto stack.
*/
void getch_(pEnv env)
{
    NULLARY(CHAR_NEWNODE, getch(env));
}
#endif
