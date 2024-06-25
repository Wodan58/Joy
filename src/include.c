/*
    module  : include.c
    version : 1.12
    date    : 06/21/24
*/
#ifndef INCLUDE_C
#define INCLUDE_C

/**
OK 3110  include  :  "filnam.ext"  ->
Transfers input to file whose name is "filnam.ext".
On end-of-file returns to previous input file.
*/
void include_(pEnv env)
{
    char *str;

    ONEPARAM("include");
    STRING("include");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    if (include(env, str))
	execerror("valid file name", "include");
    POP(env->stck);
} 
#endif
