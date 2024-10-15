/*
    module  : treerec.c
    version : 1.10
    date    : 10/11/24
*/
#ifndef TREEREC_C
#define TREEREC_C

#include "cons.c"
#include "treerecaux.c"

/**
Q2  OK  2880  treerec  :  T [O] [C]  ->  ...
T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C.
*/
void treerec_(pEnv env)
{
    THREEPARAMS("treerec");
    TWOQUOTES("treerec");
    cons_(env);
    treerecaux_(env);
}
#endif
