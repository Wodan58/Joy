/*
    module  : quit.c
    version : 1.4
    date    : 08/13/23
*/
#ifndef QUIT_C
#define QUIT_C

static int exit_index;
static void (*table[DISPLAYMAX])(pEnv);

PUBLIC void my_atexit(void (*proc)(pEnv))
{
#if 0
    if (exit_index == DISPLAYMAX)
	return;
#endif
    table[exit_index++] = proc;
}

PRIVATE void my_exit(pEnv env)
{
    while (--exit_index >= 0)
	(*table[exit_index])(env);
}

/**
OK 3160  quit  :  ->
Exit from Joy.
*/
PRIVATE void quit_(pEnv env)
{
    my_exit(env);
    exit(EXIT_SUCCESS);
}
#endif
