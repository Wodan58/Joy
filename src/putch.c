/*
    module  : putch.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef PUTCH_C
#define PUTCH_C

/**
OK 3090  putch  :  N  ->
N : numeric, writes character whose ASCII is N.
*/
USETOP(
    putch_, "putch", NUMERICTYPE, printf("%c", (int)nodevalue(env->stck).num))


#endif
