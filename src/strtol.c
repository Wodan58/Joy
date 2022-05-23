/*
    module  : strtol.c
    version : 1.2
    date    : 05/17/22
*/
#ifndef STRTOL_C
#define STRTOL_C

/**
1740  strtol  :  S I  ->  J
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
    UNARY(INTEGER_NEWNODE, strtol(nodevalue(env->stck).str, 0, base));
}
#endif
