/*
    module  : strftime.c
    version : 1.9
    date    : 10/18/24
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
    size_t leng;
    char *fmt, *result;

    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = GETSTRING(env->stck);
    POP(env->stck);
    LIST("strftime");
    decode_time(env, &t);
    leng = BUFFERMAX;
#ifdef NOBDW
    result = malloc(leng + 1);
#else
    result = GC_malloc_atomic(leng + 1);
#endif
    strftime(result, leng, fmt, &t);
    UNARY(STRING_NEWNODE, result);
#ifdef NOBDW
    free(result);
#endif
}
#endif
