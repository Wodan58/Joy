/*
    module  : include.c
    version : 1.8
    date    : 02/01/24
*/
#ifndef INCLUDE_C
#define INCLUDE_C

/**
OK 3110  include  :  "filnam.ext"  ->
Transfers input to file whose name is "filnam.ext".
On end-of-file returns to previous input file.
*/
USETOP2(include_, "include", STRING, include(env, nodevalue(env->stck).str))


#endif
