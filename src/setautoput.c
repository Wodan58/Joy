/*
    module  : setautoput.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef SETAUTOPUT_C
#define SETAUTOPUT_C

/**
OK 2980 setautoput  :  I  ->
Sets value of flag for automatic put to I (if I = 0, none;
if I = 1, put; if I = 2, stack).
*/
USETOP(
    setautoput_, "setautoput", NUMERICTYPE, env->autoput = nodevalue(env->stck).num)

#endif
