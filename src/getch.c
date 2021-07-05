/*
    module  : getch.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef GETCH_C
#define GETCH_C

/**
3100  getch  :  ->  N
Reads a character from input and puts it onto stack.
*/
PRIVATE void getch_(pEnv env) { NULLARY(CHAR_NEWNODE, getchar()); }



#endif
