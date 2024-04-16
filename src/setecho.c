/*
    module  : setecho.c
    version : 1.9
    date    : 04/11/24
*/
#ifndef SETECHO_C
#define SETECHO_C

/**
IGNORE_POP  3000  setecho  :  I  ->
[IMPURE] Sets value of echo flag for listing.
I = 0: no echo, 1: echo, 2: with tab, 3: and linenumber.
*/
void setecho_(pEnv env)
{
    ONEPARAM("setecho");
    NUMERICTYPE("setecho");
    env->echoflag = nodevalue(env->stck).num;
    POP(env->stck);
}
#endif
