/*
    module  : putch.c
    version : 1.6
    date    : 03/21/24
*/
#ifndef PUTCH_C
#define PUTCH_C

/**
IGNORE_POP  3090  putch  :  N  ->
[IMPURE] N : numeric, writes character whose ASCII is N.
*/
USETOP(putch_, "putch", NUMERICTYPE,
	printf("%c", (int)nodevalue(env->stck).num))


#endif
