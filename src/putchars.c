/*
    module  : putchars.c
    version : 1.8
    date    : 09/17/24
*/
#ifndef PUTCHARS_C
#define PUTCHARS_C

/**
Q0  IGNORE_POP  3100  putchars  :  "abc.."  ->
[IMPURE] Writes abc.. (without quotes)
*/
USETOP(putchars_, "putchars", STRING,
		printf("%s", (char *)&nodevalue(env->stck)))


#endif
