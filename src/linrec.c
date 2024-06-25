/*
    module  : linrec.c
    version : 1.6
    date    : 06/21/24
*/
#ifndef LINREC_C
#define LINREC_C

/**
OK 2710  linrec  :  [P] [T] [R1] [R2]  ->  ...
Executes P. If that yields true, executes T.
Else executes R1, recurses, executes R2.
*/
void linrecaux(pEnv env)
{
    int result;
    env->dump1 = LIST_NEWNODE(env->stck, env->dump1);
    exeterm(env, nodevalue(SAVED4).lis);
    CHECKSTACK("linrec");
    result = nodevalue(env->stck).num;
    env->stck = DMP1;
    POP(env->dump1);
    if (result)
	exeterm(env, nodevalue(SAVED3).lis);
    else {
	exeterm(env, nodevalue(SAVED2).lis);
	linrecaux(env);
	exeterm(env, nodevalue(SAVED1).lis);
    }
}

void linrec_(pEnv env)
{
    FOURPARAMS("linrec");
    FOURQUOTES("linrec");
    SAVESTACK;
    env->stck = SAVED5;
    linrecaux(env);
    POP(env->dump);
}
#endif
