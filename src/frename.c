/*
    module  : frename.c
    version : 1.6
    date    : 06/21/24
*/
#ifndef FRENAME_C
#define FRENAME_C

/**
OK 1930  frename  :  P1 P2  ->  B
[FOREIGN] The file system object with pathname P1 is renamed to P2.
B is a boolean indicating success or failure.
*/
void frename_(pEnv env)
{
    char *p1, *p2;

    TWOPARAMS("frename");
    STRING("frename");
    STRING2("frename");
#ifdef NOBDW
    p1 = (char *)&nodevalue(nextnode1(env->stck));
    p2 = (char *)&nodevalue(env->stck);
#else
    p1 = nodevalue(nextnode1(env->stck)).str;
    p2 = nodevalue(env->stck).str;
#endif
    BINARY(BOOLEAN_NEWNODE, !rename(p1, p2));
}
#endif
