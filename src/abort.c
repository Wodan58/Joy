/*
    module  : abort.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef ABORT_C
#define ABORT_C

/**
OK 3120  abort  :  ->
Aborts execution of current Joy program, returns to Joy main cycle.
*/
PRIVATE void abort_(pEnv env) { abortexecution_(MY_ABORT); }



#endif
