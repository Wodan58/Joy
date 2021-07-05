/*
    module  : frename.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FRENAME_C
#define FRENAME_C

/**
1940  frename  :  P1 P2  ->  B
The file system object with pathname P1 is renamed to P2.
B is a boolean indicating success or failure.
*/
PRIVATE void frename_(pEnv env)
{
    TWOPARAMS("frename");
    STRING("frename");
    STRING2("frename");
    BINARY(BOOLEAN_NEWNODE,
        !rename(nodevalue(nextnode1(env->stck)).str, nodevalue(env->stck).str));
}
#endif
