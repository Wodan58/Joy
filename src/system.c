/*
    module  : system.c
    version : 1.11
    date    : 09/23/24
*/
#ifndef SYSTEM_C
#define SYSTEM_C

/**
Q0  IGNORE_POP  3020  system  :  "command"  ->
[IMPURE] Escapes to shell, executes string "command".
The string may cause execution of another program.
When that has finished, the process returns to Joy.
*/
void system_(pEnv env)
{
    char *str;

    ONEPARAM("system");
    STRING("system");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);
#else
    str = nodevalue(env->stck).str;
#endif
#ifndef WINDOWS_S
    (void)system(str);
#endif
    POP(env->stck);
}
#endif
