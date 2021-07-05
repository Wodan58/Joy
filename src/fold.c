/*
    module  : fold.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef FOLD_C
#define FOLD_C

/**
2800  fold  :  A V0 [P]  ->  V
Starting with value V0, sequentially pushes members of aggregate A
and combines with binary operator P to produce value V.
*/
PRIVATE void fold_(pEnv env)
{
    THREEPARAMS("fold");
    swapd_(env);
    step_(env);
}
#endif
