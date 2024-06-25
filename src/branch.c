/*
    module  : branch.c
    version : 1.5
    date    : 06/20/24
*/
#ifndef BRANCH_C
#define BRANCH_C

/**
OK 2590  branch  :  B [T] [F]  ->  ...
If B is true, then executes T else executes F.
*/
void branch_(pEnv env)
{
    THREEPARAMS("branch");
    TWOQUOTES("branch");
    SAVESTACK;
    env->stck = SAVED4;
    exeterm(env, nodevalue(SAVED3).num ? nodevalue(SAVED2).lis :
		    nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
