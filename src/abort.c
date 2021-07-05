/*
    module  : abort.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef ABORT_C
#define ABORT_C

/**
3150  abort  :  ->
Aborts execution of current Joy program, returns to Joy main cycle.
*/
PRIVATE void abort_(pEnv env) { abortexecution_(env); }



#endif
