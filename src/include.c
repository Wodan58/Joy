/*
    module  : include.c
    version : 1.16
    date    : 10/18/24
*/
#ifndef INCLUDE_C
#define INCLUDE_C

/**
Q0  OK  3110  include  :  "filnam.ext"  ->
[SYMBOLS] Transfers input to file whose name is "filnam.ext".
On end-of-file returns to previous input file.
*/
void include_(pEnv env)
{
    char *str;

    ONEPARAM("include");
    STRING("include");
    str = GETSTRING(env->stck);
    if (include(env, str))
	execerror(env, "valid file name", "include");
    POP(env->stck);
} 
#endif
