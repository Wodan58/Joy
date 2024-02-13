/*
    module  : setecho.c
    version : 1.7
    date    : 02/01/24
*/
#ifndef SETECHO_C
#define SETECHO_C

/**
OK 3000  setecho  :  I  ->
[IMPURE] Sets value of echo flag for listing.
I = 0: no echo, 1: echo, 2: with tab, 3: and linenumber.
*/
PRIVATE void setecho_(pEnv env)
{
    ONEPARAM("setecho");
    NUMERICTYPE("setecho");
    if (!env->ignore && !env->echoflag_set)
	env->echoflag = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
