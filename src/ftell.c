/*
    module  : ftell.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FTELL_C
#define FTELL_C

/**
2000  ftell  :  S  ->  S I
I is the current position of stream S.
*/
FILEGET(ftell_, "ftell", INTEGER_NEWNODE, ftell(nodevalue(env->stck).fil))



#endif
