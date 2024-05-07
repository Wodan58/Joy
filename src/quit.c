/*
    module  : quit.c
    version : 1.10
    date    : 04/29/24
*/
#ifndef QUIT_C
#define QUIT_C

/**
IGNORE_OK  3130  quit  :  ->
[IMPURE] Exit from Joy.
*/
void quit_(pEnv env)
{
    abortexecution_(ABORT_QUIT);
}
#endif
