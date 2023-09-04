/*
    module  : app12.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef APP12_C
#define APP12_C

/**
OK 2460  app12  :  X Y1 Y2 [P]  ->  R1 R2
Executes P twice, with Y1 and Y2, returns R1 and R2.
*/
PRIVATE void app12_(pEnv env)
{
    /*   X  Y  Z  [P]  app12  */
    FOURPARAMS("app12");
    unary2_(env);
    nextnode2(env->stck) = nextnode3(env->stck); /* delete X */
}
#endif
