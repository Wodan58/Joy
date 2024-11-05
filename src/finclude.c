/*
    module  : finclude.c
    version : 1.17
    date    : 10/18/24
*/
#ifndef FINCLUDE_C
#define FINCLUDE_C

#include "get.c"

/**
Q0  OK  3170  finclude  :  S  ->  F ...
[FOREIGN] Reads Joy source code from stream S and pushes it onto stack.
*/
void finclude_(pEnv env)
{
    char *str;

    ONEPARAM("finclude");
    STRING("finclude");
    str = GETSTRING(env->stck);		/* read file name */
    POP(env->stck);			/* remove file name from stack */
    if (include(env, str))		/* include new file */
	return;
    env->finclude_busy = 1;		/* tell scanner about finclude */
    if (setjmp(env->finclude))
	env->finclude_busy = 0;		/* done with finclude */
    else while (1)			/* read all factors from file */
        get_(env);
}
#endif
