/*
    module  : putchars.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef PUTCHARS_C
#define PUTCHARS_C

/**
OK 3130  putchars  :  "abc.."  ->
Writes abc.. (without quotes)
*/
USETOP(putchars_, "putchars", STRING, printf("%s", nodevalue(env->stck).str))



#endif
