/*
    module  : ftell.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef FTELL_C
#define FTELL_C

/**
OK 1990  ftell  :  S  ->  S I
I is the current position of stream S.
*/
FILEGET(ftell_, "ftell", INTEGER_NEWNODE, ftell(nodevalue(env->stck).fil))



#endif
