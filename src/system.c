/*
    module  : system.c
    version : 1.8
    date    : 06/21/24
*/
#ifndef SYSTEM_C
#define SYSTEM_C

/**
IGNORE_POP  3020  system  :  "command"  ->
[IMPURE] Escapes to shell, executes string "command".
The string may cause execution of another program.
When that has finished, the process returns to Joy.
*/
void system_(pEnv env)
{
    int rv;
    char *str;

    ONEPARAM("system");
    STRING("system");
#ifdef NOBWD
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
    if ((rv = system(str)) != 0) {
	fflush(stdout);
	fprintf(stderr, "system: %u\n", rv & 0xFF);
    }
    POP(env->stck);
}
#endif
