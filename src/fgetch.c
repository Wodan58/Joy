/*
    module  : fgetch.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef FGETCH_C
#define FGETCH_C

/**
OK 1880  fgetch  :  S  ->  S C
C is the next available character from stream S.
*/
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, getc(nodevalue(env->stck).fil))



#endif
