/*
    module  : setautoput.c
    version : 1.2
    date    : 04/11/22
*/
#ifndef SETAUTOPUT_C
#define SETAUTOPUT_C

/**
3000 setautoput  :  I  ->
Sets value of flag for automatic put to I (if I = 0, none;
if I = 1, put; if I = 2, stack).
*/
USETOP(
    setautoput_, "setautoput", NUMERICTYPE, env->autoput = nodevalue(env->stck).num)



#endif
