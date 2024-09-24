/*
    module  : unary3.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef UNARY3_C
#define UNARY3_C

/**
Q1  OK  2510  unary3  :  X1 X2 X3 [P]  ->  R1 R2 R3
Executes P three times, with Xi, returns Ri (i = 1..3).
*/
void unary3_(pEnv env)
{ /*  X Y Z [P]  unary3    ==>  X' Y' Z'  */
    FOURPARAMS("unary3");
    ONEQUOTE("unary3");
    SAVESTACK;
    env->stck = nextnode1(SAVED3);	/* just X on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save P(X) */
    env->stck = newnode2(env, SAVED3, nextnode1(SAVED4)); /* just Y on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save P(Y) */
    env->stck = newnode2(env, SAVED2, nextnode1(SAVED4)); /* just Z on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save P(Z) */
    env->stck = env->dump1;
    env->dump1 = nextnode3(env->dump1);
    nextnode3(env->stck) = SAVED5;
    POP(env->dump);
}
#endif
