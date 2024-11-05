/*
    module  : system.c
    version : 1.14
    date    : 10/18/24
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
#ifndef WINDOWS_S
    char *str;
#endif
    ONEPARAM("system");
    STRING("system");
#ifndef WINDOWS_S
    str = GETSTRING(env->stck);
    (void)system(str);
#endif
    POP(env->stck);
}
#endif
