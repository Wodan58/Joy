/*
    module  : strtol.c
    version : 1.1
    date    : 05/21/21
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
    STRING("strtol"); /* BIT_32 */
    UNARY(INTEGER_NEWNODE, strtol(nodevalue(env->stck).str, NULL, base));
}
#endif
