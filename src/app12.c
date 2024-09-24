/*
    module  : app12.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef APP12_C
#define APP12_C

/**
Q1  OK  2460  app12  :  X Y1 Y2 [P]  ->  R1 R2
Executes P twice, with Y1 and Y2, returns R1 and R2.
*/
void app12_(pEnv env)
{
    /*   X  Y  Z  [P]  app12  */
    FOURPARAMS("app12");
    unary2_(env);
    nextnode2(env->stck) = nextnode3(env->stck); /* delete X */
}
#endif
