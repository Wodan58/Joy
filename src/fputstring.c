/*
    module  : fputstring.c
    version : 1.7
    date    : 10/11/24
*/
#ifndef FPUTSTRING_C
#define FPUTSTRING_C

#include "fputchars.c"

/**
Q0  OK  1970  fputstring  :  S "abc.."  ->  S
[FOREIGN] == fputchars, as a temporary alternative.
*/
void fputstring_(pEnv env)
{
    fputchars_(env);
}
#endif
