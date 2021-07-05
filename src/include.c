/*
    module  : include.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef INCLUDE_C
#define INCLUDE_C

/**
3140  include  :  "filnam.ext"  ->
Transfers input to file whose name is "filnam.ext".
On end-of-file returns to previous input file.
*/
USETOP(include_, "include", STRING, doinclude(env, nodevalue(env->stck).str))



#endif
