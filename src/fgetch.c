/*
    module  : fgetch.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FGETCH_C
#define FGETCH_C

/**
1880  fgetch  :  S  ->  S C
C is the next available character from stream S.
*/
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, getc(nodevalue(env->stck).fil))



#endif
