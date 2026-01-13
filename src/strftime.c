/*
    module  : strftime.c
    version : 1.11
    date    : 01/12/26
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
#ifdef NOBDW
    str = check_malloc(BUFFERMAX);
#else
    str = GC_malloc_atomic(BUFFERMAX);
#endif
    strftime(str, BUFFERMAX, fmt, &t);
    UNARY(STRING_NEWNODE, str);
#ifdef NOBDW
    free(str);
#endif
}
#endif
