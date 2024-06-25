/*
    module  : treerec.c
    version : 1.8
    date    : 03/21/24
*/
#ifndef TREEREC_C
#define TREEREC_C

/**
OK 2880  treerec  :  T [O] [C]  ->  ...
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
