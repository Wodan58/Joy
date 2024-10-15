/*
    module  : construct.c
    version : 1.8
    date    : 10/09/24
*/
#ifndef CONSTRUCT_C
#define CONSTRUCT_C

/**
Q2  OK  2470  construct  :  [P] [[P1] [P2] ..]  ->  R1 R2 ..
Saves state of stack and then executes [P].
Then executes each [Pi] to give Ri pushed onto saved stack.
*/
void construct_(pEnv env)
{	/*  [P] [[P1] [P2] ..]  ->  X1 X2 ..  */
    TWOPARAMS("construct");
    TWOQUOTES("construct");
    SAVESTACK;
    env->stck = SAVED3;					/* pop progs */
    env->dump1 = LIST_NEWNODE(env->dump2, env->dump1);	/* save env->dump2 */
    env->dump2 = env->stck;				/* save old stack */
    exeterm(env, nodevalue(SAVED2).lis);		/* [P] */
    env->dump3 = LIST_NEWNODE(env->stck, env->dump3);	/* save new stack */
    env->dump4 = LIST_NEWNODE(nodevalue(SAVED1).lis, env->dump4);
    for (; DMP4; DMP4 = nextnode1(DMP4)) {		/* step [..] */
	exeterm(env, nodevalue(DMP4).lis);
	env->dump2 = newnode2(env, env->stck, env->dump2);	/* result */
	env->stck = DMP3;				/* restore new stack */
    }
    POP(env->dump4);
    POP(env->dump3);
    env->stck = env->dump2;
    env->dump2 = nodevalue(env->dump1).lis;		/* restore old stack */
    POP(env->dump1);
    POP(env->dump);
}
#endif
