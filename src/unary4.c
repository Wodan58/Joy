/*
    module  : unary4.c
    version : 1.6
    date    : 09/17/24
*/
#ifndef UNARY4_C
#define UNARY4_C

/**
Q1  OK  2520  unary4  :  X1 X2 X3 X4 [P]  ->  R1 R2 R3 R4
Executes P four times, with Xi, returns Ri (i = 1..4).
*/
void unary4_(pEnv env)
{ /*  X Y Z W [P]  unary4    ==>  X' Y' Z' W'  */
    FIVEPARAMS("unary4");
    ONEQUOTE("unary4");
    SAVESTACK;
    env->stck = nextnode1(SAVED4);	/* just X on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save p(X) */
    env->stck = newnode2(env, SAVED4, nextnode1(SAVED5)); /* just Y on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save P(Y) */
    env->stck = newnode2(env, SAVED3, nextnode1(SAVED5)); /* just Z on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save P(Z) */
    env->stck = newnode2(env, SAVED2, nextnode1(SAVED5)); /* just W on top */
    exeterm(env, nodevalue(SAVED1).lis);	/* execute P */
    env->dump1 = newnode2(env, env->stck, env->dump1);	/* save P(W) */
    env->stck = env->dump1;
    env->dump1 = nextnode4(env->dump1);
    nextnode4(env->stck) = SAVED6;
    POP(env->dump);
}
#endif
