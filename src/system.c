/*
    module  : system.c
    version : 1.6
    date    : 02/13/24
*/
#ifndef SYSTEM_C
#define SYSTEM_C

/**
OK 3020  system  :  "command"  ->
[IMPURE] Escapes to shell, executes string "command".
The string may cause execution of another program.
When that has finished, the process returns to Joy.
*/
PRIVATE void system_(pEnv env)
{
    int rv;

    ONEPARAM("system");
    STRING("system");
    if (!env->ignore) {
	if ((rv = system(nodevalue(env->stck).str)) != 0) {
	    fflush(stdout);
	    fprintf(stderr, "system: %d\n", rv);
	}
    }
    POP(env->stck);
}
#endif
