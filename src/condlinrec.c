/*
    module  : condlinrec.c
    version : 1.8
    date    : 11/06/24
*/
#ifndef CONDLINREC_C
#define CONDLINREC_C

#include "condnestrec.c"

/**
Q1  OK  2760  condlinrec  :  [ [C1] [C2] .. [D] ]  ->  ...
Each [Ci] is of the form [[B] [T]] or [[B] [R1] [R2]].
Tries each B. If that yields true and there is just a [T], executes T and exit.
If there are [R1] and [R2], executes R1, recurses, executes R2.
Subsequent case are ignored. If no B yields true, then [D] is used.
It is then of the form [[T]] or [[R1] [R2]]. For the former, executes T.
For the latter executes R1, recurses, executes R2.
*/
void condlinrec_(pEnv env)
{
    ONEPARAM("condlinrec");
    LIST("condlinrec");
    CHECKEMPTYLIST(nodevalue(env->stck).lis, "condlinrec");
    SAVESTACK;
    env->stck = SAVED2;
    condnestrecaux(env);
    POP(env->dump);
}
#endif
