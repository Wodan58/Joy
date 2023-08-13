/*
    module  : getenv.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef GETENV_C
#define GETENV_C

/**
OK 3050  getenv  :  "variable"  ->  "value"
Retrieves the value of the environment variable "variable".
*/
PRIVATE void getenv_(pEnv env)
{
    char *str;

    ONEPARAM("getenv");
    STRING("getenv");
    if ((str = getenv(nodevalue(env->stck).str)) == 0)
        str = "";
    UNARY(STRING_NEWNODE, str);
}
#endif
