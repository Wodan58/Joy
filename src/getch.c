/*
    module  : getch.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef GETCH_C
#define GETCH_C

/**
OK 3200  getch  :  ->  N
[EXT] Reads a character from input and puts it onto stack.
*/
PRIVATE void getch_(pEnv env) { NULLARY(CHAR_NEWNODE, getchar()); }



#endif
