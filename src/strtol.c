/*
    module  : strtol.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef STRTOL_C
#define STRTOL_C

/**
Q0  OK  1740  strtol  :  S I  ->  J
String S is converted to the integer J using base I.
If I = 0, assumes base 10,
but leading "0" means base 8 and leading "0x" means base 16.
*/
void strtol_(pEnv env)
{
    int base;
    char *str;

    TWOPARAMS("strtol");
    INTEGER("strtol");
    base = nodevalue(env->stck).num;
    POP(env->stck);
    STRING("strtol");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    UNARY(INTEGER_NEWNODE, strtoll(str, 0, base));
}
#endif
