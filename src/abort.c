/*
    module  : abort.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef ABORT_C
#define ABORT_C

/**
OK 3150  abort  :  ->
Aborts execution of current Joy program, returns to Joy main cycle.
*/
PRIVATE void abort_(pEnv env) { abortexecution_(); }



#endif
