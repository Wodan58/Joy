/*
    module  : setundeferror.c
    version : 1.7
    date    : 02/01/24
*/
#ifndef SETUNDEFERROR_C
#define SETUNDEFERROR_C

/**
OK 2990  setundeferror  :  I  ->
[IMPURE] Sets flag that controls behavior of undefined functions
(0 = no error, 1 = error).
*/
PRIVATE void setundeferror_(pEnv env)
{
    ONEPARAM("setundeferror");
    NUMERICTYPE("setundeferror");
    if (!env->ignore && !env->undeferror_set)
	env->undeferror = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
