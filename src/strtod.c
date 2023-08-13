/*
    module  : strtod.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef STRTOD_C
#define STRTOD_C

/**
OK 1750  strtod  :  S  ->  R
String S is converted to the float R.
*/
PRIVATE void strtod_(pEnv env)
{
    ONEPARAM("strtod");
    STRING("strtod");
    UNARY(FLOAT_NEWNODE, strtod(nodevalue(env->stck).str, 0));
}
#endif
