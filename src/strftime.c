/*
    module  : strftime.c
    version : 1.12
    date    : 02/04/26
*/
#ifndef STRFTIME_C
#define STRFTIME_C

#include "decode.h"

/**
Q0  OK  1730  strftime  :  T S1  ->  S2
Formats a list T in the format of localtime or gmtime
using string S1 and pushes the result S2.
*/
void strftime_(pEnv env)
{
    struct tm t;
    char *fmt, *str;

    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = GETSTRING(env->stck);
    POP(env->stck);
    LIST("strftime");
    decode_time(env, &t);
    str = GC_malloc_atomic(BUFFERMAX);
    strftime(str, BUFFERMAX, fmt, &t);
    UNARY(STRING_NEWNODE, str);
}
#endif
