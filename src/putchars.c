/*
    module  : putchars.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef PUTCHARS_C
#define PUTCHARS_C

/**
3130  putchars  :  "abc.."  ->
Writes abc.. (without quotes)
*/
USETOP(putchars_, "putchars", STRING, printf("%s", nodevalue(env->stck).str))



#endif
