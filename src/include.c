/*
    module  : include.c
    version : 1.11
    date    : 05/28/24
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
    ONEPARAM("include");
    STRING("include");
    if (include(env, nodevalue(env->stck).str))
	execerror("valid file name", "include");
    POP(env->stck);
} 
#endif
