/*
    module  : getenv.c
    version : 1.9
    date    : 10/18/24
*/
#ifndef GETENV_C
#define GETENV_C

/**
Q0  OK  3030  getenv  :  "variable"  ->  "value"
[RUNTIME] Retrieves the value of the environment variable "variable".
*/
void getenv_(pEnv env)
{
    char *str;

    ONEPARAM("getenv");
    STRING("getenv");
    str = GETSTRING(env->stck);
    if ((str = getenv(str)) == 0)
	str = "";
    UNARY(STRING_NEWNODE, str);
}
#endif
