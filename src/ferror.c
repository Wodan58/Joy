/*
    module  : ferror.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef FERROR_C
#define FERROR_C

/**
OK 1850  ferror  :  S  ->  S B
B is the error status of stream S.
*/
FILEGET(ferror_, "ferror", BOOLEAN_NEWNODE, ferror(nodevalue(env->stck).fil))



#endif
