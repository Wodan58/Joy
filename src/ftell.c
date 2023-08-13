/*
    module  : ftell.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef FTELL_C
#define FTELL_C

/**
OK 2000  ftell  :  S  ->  S I
I is the current position of stream S.
*/
FILEGET(ftell_, "ftell", INTEGER_NEWNODE, ftell(nodevalue(env->stck).fil))



#endif
