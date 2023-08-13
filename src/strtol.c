/*
    module  : strtol.c
    version : 1.4
    date    : 08/13/23
*/
#ifndef STRTOL_C
#define STRTOL_C

/**
OK 1740  strtol  :  S I  ->  J
String S is converted to the integer J using base I.
If I = 0, assumes base 10,
but leading "0" means base 8 and leading "0x" means base 16.
*/
PRIVATE void strtol_(pEnv env)
{
    int base;

    TWOPARAMS("strtol");
    INTEGER("strtol");
    base = nodevalue(env->stck).num;
    POP(env->stck);
    STRING("strtol");
    UNARY(INTEGER_NEWNODE, strtoll(nodevalue(env->stck).str, 0, base));
}
#endif
