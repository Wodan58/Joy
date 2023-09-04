/*
    module  : frename.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef FRENAME_C
#define FRENAME_C

/**
OK 1930  frename  :  P1 P2  ->  B
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
