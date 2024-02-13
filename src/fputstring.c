/*
    module  : fputstring.c
    version : 1.4
    date    : 01/17/24
*/
#ifndef FPUTSTRING_C
#define FPUTSTRING_C

/**
OK 1970  fputstring  :  S "abc.."  ->  S
[FOREIGN] == fputchars, as a temporary alternative.
*/
PRIVATE void fputstring_(pEnv env) { fputchars_(env); }



#endif
