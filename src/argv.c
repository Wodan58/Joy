/*
    module  : argv.c
    version : 1.7
    date    : 09/17/24
*/
#ifndef ARGV_C
#define ARGV_C

/**
Q0  OK  3040  argv  :  ->  A
Creates an aggregate A containing the interpreter's command line arguments.
*/
void argv_(pEnv env)
{
    int i;
    Index temp;

    env->dump1 = LIST_NEWNODE(0, env->dump1);
    for (i = env->g_argc - 1; i >= 0; i--) {
	temp = STRING_NEWNODE(env->g_argv[i], DMP1);
	DMP1 = temp;
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
}
#endif
