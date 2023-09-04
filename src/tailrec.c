/*
    module  : tailrec.c
    version : 1.4
    date    : 09/04/23
*/
#ifndef TAILREC_C
#define TAILREC_C

/**
OK 2720  tailrec  :  [P] [T] [R1]  ->  ...
Executes P. If that yields true, executes T.
Else executes R1, recurses.
*/
PRIVATE void tailrecaux(pEnv env)
{
    int result;
tailrec:
    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED3).lis);
    CHECKSTACK("tailrec");
    result = nodevalue(env->stck).num;
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
        exeterm(env, nodevalue(SAVED2).lis);
    else {
        exeterm(env, nodevalue(SAVED1).lis);
        goto tailrec;
    }
}

PRIVATE void tailrec_(pEnv env)
{
    THREEPARAMS("tailrec");
    THREEQUOTES("tailrec");
    SAVESTACK;
    env->stck = SAVED4;
    tailrecaux(env);
    POP(env->dump);
}
#endif
