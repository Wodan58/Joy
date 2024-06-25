/*
    module  : fremove.c
    version : 1.6
    date    : 06/21/24
*/
#ifndef FREMOVE_C
#define FREMOVE_C

/**
OK 1920  fremove  :  P  ->  B
[FOREIGN] The file system object with pathname P is removed from the file
system. B is a boolean indicating success or failure.
*/
void fremove_(pEnv env)
{
    char *str;

    ONEPARAM("fremove");
    STRING("fremove");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    UNARY(BOOLEAN_NEWNODE, !remove(str));
}
#endif
