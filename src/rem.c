/*
    module  : rem.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef REM_C
#define REM_C

/**
1420  rem  :  I J  ->  K
Integer K is the remainder of dividing I by J.  Also supports float.
*/
PRIVATE void rem_(pEnv env)
{
    TWOPARAMS("rem");
    FLOAT_P(fmod);
    INTEGERS2("rem");
    CHECKZERO("rem");
    BINARY(INTEGER_NEWNODE,
        nodevalue(nextnode1(env->stck)).num % nodevalue(env->stck).num);
}
#endif
