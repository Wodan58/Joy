/*
    module  : cleave.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef CLEAVE_C
#define CLEAVE_C

/**
OK 2580  cleave  :  X [P1] [P2]  ->  R1 R2
Executes P1 and P2, each with X on top, producing two results.
*/
PRIVATE void cleave_(pEnv env)
{ /*  X [P1] [P2] cleave ==>  X1 X2        */
    THREEPARAMS("cleave");
    TWOQUOTES("cleave");
    SAVESTACK;
    env->stck = SAVED3;
    exeterm(env, nodevalue(SAVED2).lis); /* [P1]                */
    env->dump1 = newnode(
        env, nodetype(env->stck), nodevalue(env->stck), env->dump1); /*  X1 */
    env->stck = SAVED3;
    exeterm(env, nodevalue(SAVED1).lis); /* [P2]                */
    env->dump1 = newnode(
        env, nodetype(env->stck), nodevalue(env->stck), env->dump1); /*  X2 */
    env->stck = env->dump1;
    env->dump1 = nextnode2(env->dump1);
    nextnode2(env->stck) = SAVED4;
    POP(env->dump);
}
#endif
