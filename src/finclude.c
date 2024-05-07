/*
    module  : finclude.c
    version : 1.12
    date    : 05/02/24
*/
#ifndef FINCLUDE_C
#define FINCLUDE_C

/**
OK 3160  finclude  :  S  ->  F ...
[FOREIGN] Reads Joy source code from stream S and pushes it onto stack.
*/
void finclude_(pEnv env)
{
    char *str;

    ONEPARAM("finclude");
    STRING("finclude");
    str = nodevalue(env->stck).str;	/* read file name */
    POP(env->stck);			/* remove file name from stack */
    include(env, str);			/* include new file */
    env->finclude_busy = 1;		/* tell scanner about finclude */
    if (setjmp(env->finclude))
	env->finclude_busy = 0;		/* done with finclude */
    else while (1)
	get_(env);			/* read all factors from file */
}
#endif
