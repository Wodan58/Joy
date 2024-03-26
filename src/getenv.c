/*
    module  : getenv.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef GETENV_C
#define GETENV_C

/**
OK 3030  getenv  :  "variable"  ->  "value"
Retrieves the value of the environment variable "variable".
*/
void getenv_(pEnv env)
{
    char *str;

    ONEPARAM("getenv");
    STRING("getenv");
    if ((str = getenv(nodevalue(env->stck).str)) == 0)
        str = "";
    UNARY(STRING_NEWNODE, str);
}
#endif
