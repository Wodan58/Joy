/*
    module  : strftime.c
    version : 1.4
    date    : 08/18/23
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
    size_t leng;
    char *fmt, *result;

    TWOPARAMS("strftime");
    STRING("strftime");
    fmt = nodevalue(env->stck).str;
    POP(env->stck);
    LIST("strftime");
    decode_time(env, &t);
    leng = BUFFERMAX;
    result = GC_malloc_atomic(leng + 1);
    strftime(result, leng, fmt, &t);
    UNARY(STRING_NEWNODE, result);
}
#endif
