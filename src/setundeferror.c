/*
    module  : setundeferror.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef SETUNDEFERROR_C
#define SETUNDEFERROR_C

/**
IGNORE_POP  2990  setundeferror  :  I  ->
[IMPURE] Sets flag that controls behavior of undefined functions
(0 = no error, 1 = error).
*/
void setundeferror_(pEnv env)
{
    ONEPARAM("setundeferror");
    NUMERICTYPE("setundeferror");
    if (!env->undeferror_set)
	env->undeferror = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
