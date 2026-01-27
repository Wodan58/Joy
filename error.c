/*
 *  module  : error.c
 *  version : 1.4
 *  date    : 01/24/26
 */
#include "globals.h"

/*
 * print a runtime error to stderr and abort the execution of current program.
 */
void execerror(pEnv env, char *message, char *op)
{
    int leng = 0;
    char *ptr, *str;
#ifdef COMPILER
    Entry ent;

    if (env->compiling > 0) {
	leng = lookup(env, op);			/* locate in symbol table */
	ent = vec_at(env->symtab, leng);
	ent.flags |= IS_USED;
	vec_at(env->symtab, leng) = ent;
	printstack(env);
	if (leng < tablesize())
	    fprintf(env->outfp, "%s_(env);\n", nickname(leng));
	else
	    fprintf(env->outfp, "do_%s(env);\n", op);
	return;
    }
#endif
    if ((ptr = strrchr(op, '/')) != 0)
	ptr++;
    else
	ptr = op;
    if ((str = strrchr(ptr, '.')) != 0 && str[1] == 'c')
	leng = str - ptr;
    else
	leng = strlen(ptr);
    fflush(stdout);
    fprintf(stderr, "run time error: %s needed for %.*s\n", message, leng, ptr);
    abortexecution_(ABORT_RETRY);
}	/* LCOV_EXCL_LINE */
