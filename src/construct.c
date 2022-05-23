/*
    module  : construct.c
    version : 1.2
    date    : 05/17/22
*/
#ifndef CONSTRUCT_C
#define CONSTRUCT_C

/**
2490  construct  :  [P] [[P1] [P2] ..]  ->  R1 R2 ..
Saves state of stack and then executes [P].
Then executes each [Pi] to give Ri pushed onto saved stack.
*/
PRIVATE void construct_(pEnv env)
{ /* [P] [[P1] [P2] ..] -> X1 X2 ..        */
    TWOPARAMS("construct");
    TWOQUOTES("construct");
    SAVESTACK;
    env->stck = SAVED3; /* pop progs                */
    env->dump1 = LIST_NEWNODE(env->dump2, env->dump1); /* save env->dump2 */
    env->dump2 = env->stck; /* save old stack        */
    exeterm(env, nodevalue(SAVED2).lis); /* [P]                        */
    env->dump3 = LIST_NEWNODE(env->stck, env->dump3); /* save current stack */
    env->dump4
        = newnode(env, LIST_, nodevalue(SAVED1), env->dump4); /* step [..] */
    while (DMP4) {
        env->stck = DMP3; /* restore new stack        */
        exeterm(env, nodevalue(DMP4).lis);
        env->dump2 = newnode(env, nodetype(env->stck), nodevalue(env->stck),
            env->dump2); /* result        */
        DMP4 = nextnode1(DMP4);
    }
    POP(env->dump4);
    POP(env->dump3);
    env->stck = env->dump2;
    env->dump2 = nodevalue(env->dump1).lis; /* restore env->dump2        */
    POP(env->dump1);
    POP(env->dump);
}
#endif
