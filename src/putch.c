/*
    module  : putch.c
    version : 1.5
    date    : 01/22/24
*/
#ifndef PUTCH_C
#define PUTCH_C

/**
OK 3090  putch  :  N  ->
[IMPURE] N : numeric, writes character whose ASCII is N.
*/
USETOP(putch_, "putch", NUMERICTYPE,
	printf("%c", (int)nodevalue(env->stck).num))


#endif
