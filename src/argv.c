/*
    module  : argv.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef ARGV_C
#define ARGV_C

/**
OK 3060  argv  :  ->  A
Creates an aggregate A containing the interpreter's command line arguments.
*/
PRIVATE void argv_(pEnv env)
{
    int i;
    Index *my_dump;

    NULLARY(LIST_NEWNODE, 0);
    my_dump = &nodevalue(env->stck).lis;
    for (i = 0; i < env->g_argc; i++) {
        *my_dump = STRING_NEWNODE(env->g_argv[i], 0);
        my_dump = &nextnode1(*my_dump);
    }
}
#endif
