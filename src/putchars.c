/*
    module  : putchars.c
    version : 1.6
    date    : 03/21/24
*/
#ifndef PUTCHARS_C
#define PUTCHARS_C

/**
IGNORE_POP  3100  putchars  :  "abc.."  ->
[IMPURE] Writes abc.. (without quotes)
*/
USETOP(putchars_, "putchars", STRING, printf("%s", nodevalue(env->stck).str))



#endif
