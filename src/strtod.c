/*
    module  : strtod.c
    version : 1.5
    date    : 06/21/24
*/
#ifndef STRTOD_C
#define STRTOD_C

/**
OK 1750  strtod  :  S  ->  R
String S is converted to the float R.
*/
void strtod_(pEnv env)
{
    char *str;

    ONEPARAM("strtod");
    STRING("strtod");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    UNARY(FLOAT_NEWNODE, strtod(str, 0));
}
#endif
