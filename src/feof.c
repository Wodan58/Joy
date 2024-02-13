/*
    module  : feof.c
    version : 1.3
    date    : 01/17/24
*/
#ifndef FEOF_C
#define FEOF_C

/**
OK 1840  feof  :  S  ->  S B
[FOREIGN] B is the end-of-file status of stream S.
*/
FILEGET(feof_, "feof", BOOLEAN_NEWNODE, feof(nodevalue(env->stck).fil))



#endif
