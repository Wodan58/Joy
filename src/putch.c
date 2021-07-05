/*
    module  : putch.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef PUTCH_C
#define PUTCH_C

/**
3120  putch  :  N  ->
N : numeric, writes character whose ASCII is N.
*/
USETOP(
    putch_, "putch", NUMERICTYPE, printf("%c", (int)nodevalue(env->stck).num))



#endif
