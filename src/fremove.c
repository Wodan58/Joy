/*
    module  : fremove.c
    version : 1.8
    date    : 10/18/24
*/
#ifndef FREMOVE_C
#define FREMOVE_C

/**
Q0  OK  1920  fremove  :  P  ->  B
[FOREIGN] The file system object with pathname P is removed from the file
system. B is a boolean indicating success or failure.
*/
void fremove_(pEnv env)
{
    char *str;

    ONEPARAM("fremove");
    STRING("fremove");
    str = GETSTRING(env->stck);
    UNARY(BOOLEAN_NEWNODE, !remove(str));
}
#endif
