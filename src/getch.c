/*
    module  : getch.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef GETCH_C
#define GETCH_C

/**
OK 3100  getch  :  ->  N
Reads a character from input and puts it onto stack.
*/
PRIVATE void getch_(pEnv env) { NULLARY(CHAR_NEWNODE, getchar()); }



#endif
