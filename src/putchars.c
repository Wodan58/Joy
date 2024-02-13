/*
    module  : putchars.c
    version : 1.5
    date    : 01/22/24
*/
#ifndef PUTCHARS_C
#define PUTCHARS_C

/**
OK 3100  putchars  :  "abc.."  ->
[IMPURE] Writes abc.. (without quotes)
*/
USETOP(putchars_, "putchars", STRING, printf("%s", nodevalue(env->stck).str))



#endif
