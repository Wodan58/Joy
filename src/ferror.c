/*
    module  : ferror.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FERROR_C
#define FERROR_C

/**
1850  ferror  :  S  ->  S B
B is the error status of stream S.
*/
FILEGET(ferror_, "ferror", BOOLEAN_NEWNODE, ferror(nodevalue(env->stck).fil))



#endif
