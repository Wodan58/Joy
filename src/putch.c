/*
    module  : putch.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef PUTCH_C
#define PUTCH_C

/**
OK 3120  putch  :  N  ->
N : numeric, writes character whose ASCII is N.
*/
USETOP(
    putch_, "putch", NUMERICTYPE, printf("%c", (int)nodevalue(env->stck).num))


#endif
