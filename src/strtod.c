/*
    module  : strtod.c
    version : 1.7
    date    : 10/18/24
*/
#ifndef STRTOD_C
#define STRTOD_C

/**
Q0  OK  1750  strtod  :  S  ->  R
String S is converted to the float R.
*/
void strtod_(pEnv env)
{
    char *str;

    ONEPARAM("strtod");
    STRING("strtod");
    str = GETSTRING(env->stck);
    UNARY(FLOAT_NEWNODE, strtod(str, 0));
}
#endif
