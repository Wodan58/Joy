/*
    module  : branch.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef BRANCH_C
#define BRANCH_C

/**
OK 2590  branch  :  B [T] [F]  ->  ...
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
