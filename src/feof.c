/*
    module  : feof.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef FEOF_C
#define FEOF_C

/**
OK 1840  feof  :  S  ->  S B
B is the end-of-file status of stream S.
*/
FILEGET(feof_, "feof", BOOLEAN_NEWNODE, feof(nodevalue(env->stck).fil))



#endif
