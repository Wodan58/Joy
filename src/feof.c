/*
    module  : feof.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FEOF_C
#define FEOF_C

/**
1840  feof  :  S  ->  S B
B is the end-of-file status of stream S.
*/
FILEGET(feof_, "feof", BOOLEAN_NEWNODE, feof(nodevalue(env->stck).fil))



#endif
