/*
    module  : getenv.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef GETENV_C
#define GETENV_C

/**
Q0  OK  3030  getenv  :  "variable"  ->  "value"
Retrieves the value of the environment variable "variable".
*/
void getenv_(pEnv env)
{
    char *str;

    ONEPARAM("getenv");
    STRING("getenv");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    if ((str = getenv(str)) == 0)
	str = "";
    UNARY(STRING_NEWNODE, str);
}
#endif
