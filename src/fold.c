/*
    module  : fold.c
    version : 1.6
    date    : 10/11/24
*/
#ifndef FOLD_C
#define FOLD_C

#include "swapd.c"
#include "step.c"

/**
Q1  OK  2780  fold  :  A V0 [P]  ->  V
Starting with value V0, sequentially pushes members of aggregate A
and combines with binary operator P to produce value V.
*/
void fold_(pEnv env)
{
    THREEPARAMS("fold");
    swapd_(env);
    step_(env);
}
#endif
