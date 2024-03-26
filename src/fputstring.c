/*
    module  : fputstring.c
    version : 1.5
    date    : 03/21/24
*/
#ifndef FPUTSTRING_C
#define FPUTSTRING_C

/**
OK 1970  fputstring  :  S "abc.."  ->  S
[FOREIGN] == fputchars, as a temporary alternative.
*/
void fputstring_(pEnv env) { fputchars_(env); }



#endif
