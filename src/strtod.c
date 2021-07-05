/*
    module  : strtod.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef STRTOD_C
#define STRTOD_C

/**
1750  strtod  :  S  ->  R
String S is converted to the float R.
*/
PRIVATE void strtod_(pEnv env)
{
    ONEPARAM("strtod");
    STRING("strtod");
    UNARY(FLOAT_NEWNODE, strtod(nodevalue(env->stck).str, NULL));
}
#endif
