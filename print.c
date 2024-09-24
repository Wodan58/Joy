/*
 *  module  : print.c
 *  version : 1.1
 *  date    : 09/16/24
 */
#include "globals.h"

/*
 * print the stack according to the autoput settings.
 */
void print(pEnv env)
{
    if (env->stck) {
	if (env->autoput == 2)
	    writeterm(env, env->stck, stdout);
	else if (env->autoput == 1) {
	    writefactor(env, env->stck, stdout);
	    env->stck = nextnode1(env->stck);
	}
	if (env->autoput) {
	    putchar('\n');
	    fflush(stdout);
	}
    }
}
