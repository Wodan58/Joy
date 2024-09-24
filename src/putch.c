/*
    module  : putch.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef PUTCH_C
#define PUTCH_C

/**
Q0  IGNORE_POP  3090  putch  :  N  ->
[IMPURE] N : numeric, writes character whose ASCII is N.
*/
USETOP(putch_, "putch", NUMERICTYPE,
	printf("%c", (int)nodevalue(env->stck).num))


#endif
