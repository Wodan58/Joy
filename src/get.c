/*
    module  : get.c
    version : 1.6
    date    : 01/22/24
*/
#ifndef GET_C
#define GET_C

/**
OK 3070  get  :  ->  F
[IMPURE] Reads a factor from input and pushes it onto stack.
*/
PRIVATE void get_(pEnv env)
{
    if (env->ignore) {
	env->bucket.num = 0;
	env->stck = newnode(env, INTEGER_, env->bucket, env->stck);
	return;
    }
    getsym(env);
    readfactor(env);
}
#endif
