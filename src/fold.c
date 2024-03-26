/*
    module  : fold.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef FOLD_C
#define FOLD_C

/**
OK 2780  fold  :  A V0 [P]  ->  V
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
