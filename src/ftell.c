/*
    module  : ftell.c
    version : 1.4
    date    : 01/17/24
*/
#ifndef FTELL_C
#define FTELL_C

/**
OK 1990  ftell  :  S  ->  S I
[FOREIGN] I is the current position of stream S.
*/
FILEGET(ftell_, "ftell", INTEGER_NEWNODE, ftell(nodevalue(env->stck).fil))



#endif
