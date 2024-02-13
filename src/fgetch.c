/*
    module  : fgetch.c
    version : 1.4
    date    : 01/17/24
*/
#ifndef FGETCH_C
#define FGETCH_C

/**
OK 1870  fgetch  :  S  ->  S C
[FOREIGN] C is the next available character from stream S.
*/
FILEGET(fgetch_, "fgetch", CHAR_NEWNODE, getc(nodevalue(env->stck).fil))



#endif
