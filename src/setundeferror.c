/*
    module  : setundeferror.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef SETUNDEFERROR_C
#define SETUNDEFERROR_C

/**
3010  setundeferror  :  I  ->
Sets flag that controls behavior of undefined functions
(0 = no error, 1 = error).
*/
USETOP(setundeferror_, "setundeferror", NUMERICTYPE,
    undeferror = nodevalue(env->stck).num)



#endif
