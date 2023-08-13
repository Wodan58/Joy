/*
    module  : strftime.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef STRFTIME_C
#define STRFTIME_C

#include "decode.h"

/**
OK 1730  strftime  :  T S1  ->  S2
Formats a list T in the format of localtime or gmtime
using string S1 and pushes the result S2.
*/
PRIVATE void strftime_(pEnv env)
{
    struct tm t;
    char *fmt, *result;
    size_t length;

    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = nodevalue(env->stck).str;
    POP(env->stck);
    LIST("strftime");
    decode_time(env, &t);
    length = BUFFERMAX;
    result = GC_malloc_atomic(length);
    strftime(result, length, fmt, &t);
    UNARY(STRING_NEWNODE, result);
}
#endif
