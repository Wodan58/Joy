/*
    module  : feof.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef FEOF_C
#define FEOF_C

/**
Q0  OK  1840  feof  :  S  ->  S B
[FOREIGN] B is the end-of-file status of stream S.
*/
FILEGET(feof_, "feof", BOOLEAN_NEWNODE, feof(nodevalue(env->stck).fil))



#endif
