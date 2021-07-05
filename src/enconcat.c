/*
    module  : enconcat.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef ENCONCAT_C
#define ENCONCAT_C

/**
2170  enconcat  :  X S T  ->  U
Sequence U is the concatenation of sequences S and T
with X inserted between S and T (== swapd cons concat).
*/
PRIVATE void enconcat_(pEnv env)
{
    THREEPARAMS("enconcat");
    SAME2TYPES("enconcat");
    swapd_(env);
    cons_(env);
    concat_(env);
}
#endif
