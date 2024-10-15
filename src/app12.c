/*
    module  : app12.c
    version : 1.7
    date    : 10/11/24
*/
#ifndef APP12_C
#define APP12_C

#include "unary2.c"
#include "rolldown.c"
#include "pop.c"

/**
Q1  OK  2460  app12  :  X Y1 Y2 [P]  ->  R1 R2
Executes P twice, with Y1 and Y2, returns R1 and R2.
*/
void app12_(pEnv env)
{
    /*   X  Y  Z  [P]  app12  */
    FOURPARAMS("app12");
    unary2_(env);
    rolldown_(env);
    pop_(env);
}
#endif
