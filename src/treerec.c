/*
    module  : treerec.c
    version : 1.7
    date    : 09/04/23
*/
#ifndef TREEREC_C
#define TREEREC_C

/**
OK 2880  treerec  :  T [O] [C]  ->  ...
T is a tree. If T is a leaf, executes O. Else executes [[[O] C] treerec] C.
*/
PRIVATE void treerec_(pEnv env)
{
    THREEPARAMS("treerec");
    TWOQUOTES("treerec");
    cons_(env);
    _treerec_(env);
}
#endif
