/*
    module  : unary2.c
    version : 1.4
    date    : 03/21/24
*/
#ifndef UNARY2_C
#define UNARY2_C

/**
OK 2500  unary2  :  X1 X2 [P]  ->  R1 R2
Executes P twice, with X1 and X2 on top of the stack.
Returns the two values R1 and R2.
*/
void unary2_(pEnv env)
{ /*   Y  Z  [P]  unary2     ==>  Y'  Z'  */
    THREEPARAMS("unary2");
    ONEQUOTE("unary2");
    SAVESTACK;
    env->stck = nextnode1(SAVED2); /* just Y on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Y) */
    env->stck = newnode(env, nodetype(SAVED2), nodevalue(SAVED2),
        nextnode1(SAVED3)); /* just Z on top */
    exeterm(env, nodevalue(SAVED1).lis); /* execute P */
    env->dump1 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
        env->dump1); /* save P(Z) */
    env->stck = env->dump1;
    env->dump1 = nextnode2(env->dump1);
    nextnode2(env->stck) = SAVED4;
    POP(env->dump);
}
#endif
