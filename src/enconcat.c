/*
    module  : enconcat.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef ENCONCAT_C
#define ENCONCAT_C

/**
OK 2160  enconcat  :  X S T  ->  U
Sequence U is the concatenation of sequences S and T
with X inserted between S and T (== swapd cons concat).
*/
void enconcat_(pEnv env)
{
    THREEPARAMS("enconcat");
    SAME2TYPES("enconcat");
    swapd_(env);
    cons_(env);
    concat_(env);
}
#endif
