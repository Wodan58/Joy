/*
    module  : setundeferror.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef SETUNDEFERROR_C
#define SETUNDEFERROR_C

/**
OK 2990  setundeferror  :  I  ->
Sets flag that controls behavior of undefined functions
(0 = no error, 1 = error).
*/
USETOP(setundeferror_, "setundeferror", NUMERICTYPE,
    env->undeferror = nodevalue(env->stck).num)

#endif
