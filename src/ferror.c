/*
    module  : ferror.c
    version : 1.3
    date    : 01/17/24
*/
#ifndef FERROR_C
#define FERROR_C

/**
OK 1850  ferror  :  S  ->  S B
[FOREIGN] B is the error status of stream S.
*/
FILEGET(ferror_, "ferror", BOOLEAN_NEWNODE, ferror(nodevalue(env->stck).fil))



#endif
