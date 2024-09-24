/*
    module  : setundeferror.c
    version : 1.9
    date    : 09/17/24
*/
#ifndef SETUNDEFERROR_C
#define SETUNDEFERROR_C

/**
Q0  IGNORE_POP  2990  setundeferror  :  I  ->
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
