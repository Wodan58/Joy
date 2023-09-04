/*
    module  : fgetch.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef FGETCH_C
#define FGETCH_C

/**
OK 1870  fgetch  :  S  ->  S C
C is the next available character from stream S.
*/
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, getc(nodevalue(env->stck).fil))



#endif
