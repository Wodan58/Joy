/*
    module  : abort.c
    version : 1.2
    date    : 05/02/22
*/
#ifndef ABORT_C
#define ABORT_C

/**
3150  abort  :  ->
Aborts execution of current Joy program, returns to Joy main cycle.
*/
PRIVATE void abort_(pEnv env) { abortexecution_(); }



#endif
