/*
    module  : fputstring.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef FPUTSTRING_C
#define FPUTSTRING_C

/**
Q0  OK  1970  fputstring  :  S "abc.."  ->  S
[FOREIGN] == fputchars, as a temporary alternative.
*/
void fputstring_(pEnv env)
{
    fputchars_(env);
}
#endif
