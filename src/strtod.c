/*
    module  : strtod.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef STRTOD_C
#define STRTOD_C

/**
OK 1750  strtod  :  S  ->  R
String S is converted to the float R.
*/
void strtod_(pEnv env)
{
    ONEPARAM("strtod");
    STRING("strtod");
    UNARY(FLOAT_NEWNODE, strtod(nodevalue(env->stck).str, 0));
}
#endif
