/*
    module  : system.c
    version : 1.5
    date    : 01/22/24
*/
#ifndef SYSTEM_C
#define SYSTEM_C

/**
OK 3020  system  :  "command"  ->
[IMPURE] Escapes to shell, executes string "command".
The string may cause execution of another program.
When that has finished, the process returns to Joy.
*/
USETOP(system_, "system", STRING, (void)system(nodevalue(env->stck).str))

#endif
