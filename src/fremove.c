/*
    module  : fremove.c
    version : 1.5
    date    : 03/21/24
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
    ONEPARAM("fremove");
    STRING("fremove");
    UNARY(BOOLEAN_NEWNODE, !remove(nodevalue(env->stck).str));
}
#endif
