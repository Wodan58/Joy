/*
    module  : system.c
    version : 1.15
    date    : 01/08/26
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
#ifdef ALLOW_SYSTEM_CALLS
    char *str;
#endif
    ONEPARAM("system");
    STRING("system");
#ifdef ALLOW_SYSTEM_CALLS
    str = GETSTRING(env->stck);
    (void)system(str);
#endif
    POP(env->stck);
}
#endif
