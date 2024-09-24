/*
    module  : fgetch.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef FGETCH_C
#define FGETCH_C

/**
Q0  OK  1870  fgetch  :  S  ->  S C
[FOREIGN] C is the next available character from stream S.
*/
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, getc(nodevalue(env->stck).fil))



#endif
