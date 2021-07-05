/*
    module  : branch.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef BRANCH_C
#define BRANCH_C

/**
2610  branch  :  B [T] [F]  ->  ...
If B is true, then executes T else executes F.
*/
PRIVATE void branch_(pEnv env)
{
    THREEPARAMS("branch");
    TWOQUOTES("branch");
    SAVESTACK;
    env->stck = SAVED4;
    exeterm(env,
        nodevalue(SAVED3).num ? nodevalue(SAVED2).lis : nodevalue(SAVED1).lis);
    POP(env->dump);
}
#endif
