/*
    module  : %M%
    version : %I%
    date    : %G%
*/
#ifndef ABORT_C
#define ABORT_C

/**
OK 3150  abort  :  ->
Aborts execution of current Joy program, returns to Joy main cycle.
*/
PRIVATE void abort_(pEnv env) { abortexecution_(MY_ABORT); }



#endif
