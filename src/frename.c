/*
    module  : frename.c
    version : 1.8
    date    : 10/18/24
*/
#ifndef FRENAME_C
#define FRENAME_C

/**
Q0  OK  1930  frename  :  P1 P2  ->  B
[FOREIGN] The file system object with pathname P1 is renamed to P2.
B is a boolean indicating success or failure.
*/
void frename_(pEnv env)
{
    char *p1, *p2;

    TWOPARAMS("frename");
    STRING("frename");
    STRING2("frename");
    p1 = GETSTRING(nextnode1(env->stck));
    p2 = GETSTRING(env->stck);
    BINARY(BOOLEAN_NEWNODE, !rename(p1, p2));
}
#endif
