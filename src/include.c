/*
    module  : include.c
    version : 1.5
    date    : 09/04/23
*/
#ifndef INCLUDE_C
#define INCLUDE_C

/**
OK 3110  include  :  "filnam.ext"  ->
Transfers input to file whose name is "filnam.ext".
On end-of-file returns to previous input file.
*/
USETOP(include_, "include", STRING, include(env, nodevalue(env->stck).str, 1))


#endif
