/*
    module  : setautoput.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef SETAUTOPUT_C
#define SETAUTOPUT_C

/**
3000 setautoput  :  I  ->
Sets value of flag for automatic put to I (if I = 0, none;
if I = 1, put; if I = 2, stack).
*/
USETOP(
    setautoput_, "setautoput", NUMERICTYPE, autoput = nodevalue(env->stck).num)



#endif
