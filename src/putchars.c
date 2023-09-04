/*
    module  : putchars.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef PUTCHARS_C
#define PUTCHARS_C

/**
OK 3100  putchars  :  "abc.."  ->
Writes abc.. (without quotes)
*/
USETOP(putchars_, "putchars", STRING, printf("%s", nodevalue(env->stck).str))



#endif
