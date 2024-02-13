/*
    module  : quit.c
    version : 1.8
    date    : 02/02/24
*/
#ifndef QUIT_C
#define QUIT_C

static int exit_index;
static void (*table[DISPLAYMAX])(pEnv);

/**
OK 3130  quit  :  ->
Exit from Joy.
*/
PUBLIC void my_atexit(void (*proc)(pEnv))
{
    if (exit_index == DISPLAYMAX)
	return;
    table[exit_index++] = proc;
}

PRIVATE void my_exit(pEnv env)
{
    while (--exit_index >= 0)
	(*table[exit_index])(env);
}

PRIVATE void quit_(pEnv env)
{
    my_exit(env);
    exit(EXIT_SUCCESS);
}
#endif
