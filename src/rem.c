/*
    module  : rem.c
    version : 1.5
    date    : 09/17/24
*/
#ifndef REM_C
#define REM_C

/**
Q0  OK  1420  rem  :  I J  ->  K
Integer K is the remainder of dividing I by J.  Also supports float.
*/
void rem_(pEnv env)
{
    TWOPARAMS("rem");
    FLOAT_P(fmod);
    INTEGERS2("rem");
    CHECKZERO("rem");
    BINARY(INTEGER_NEWNODE, nodevalue(nextnode1(env->stck)).num %
		    nodevalue(env->stck).num);
}
#endif
