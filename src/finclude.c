/*
    module  : finclude.c
    version : 1.15
    date    : 09/17/24
*/
#ifndef FINCLUDE_C
#define FINCLUDE_C

/**
Q0  OK  3170  finclude  :  S  ->  F ...
[FOREIGN] Reads Joy source code from stream S and pushes it onto stack.
*/
void finclude_(pEnv env)
{
    char *str;

    ONEPARAM("finclude");
    STRING("finclude");
#ifdef NOBDW
    str = (char *)&nodevalue(env->stck);/* read file name */
#else
    str = nodevalue(env->stck).str;	/* read file name */
#endif
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
