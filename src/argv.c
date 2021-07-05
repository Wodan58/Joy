/*
    module  : argv.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef ARGV_C
#define ARGV_C

/**
3060  argv  :  ->  A
Creates an aggregate A containing the interpreter's command line arguments.
*/
PRIVATE void argv_(pEnv env)
{
    int i;
    env->dump1 = LIST_NEWNODE(NULL, env->dump1);
    for (i = g_argc - 1; i >= 0; i--)
        DMP1 = STRING_NEWNODE(g_argv[i], DMP1);
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
    return;
}
#endif
