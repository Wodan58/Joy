/*
    module  : setautoput.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef SETAUTOPUT_C
#define SETAUTOPUT_C

/**
Q0  IGNORE_POP  2980  setautoput  :  I  ->
[IMPURE] Sets value of flag for automatic put to I (if I = 0, none;
if I = 1, put; if I = 2, stack).
*/
void setautoput_(pEnv env)
{
    ONEPARAM("setautoput");
    NUMERICTYPE("setautoput");
    if (!env->autoput_set)
	env->autoput = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
