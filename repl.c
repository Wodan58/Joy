/*
 *  module  : repl.c
 *  version : 1.2
 *  date    : 10/11/24
 */
#include "globals.h"

/*
 * writedump - print the contents of a dump in readable format to fp,
 *	       with a limit of 10 factors.
 */
#ifdef NOBDW
static void writedump(pEnv env, Index n, FILE *fp)
{
    int i;

    for (i = 0; n && i < 10; i++) {
	writefactor(env, n, fp);
	POP(n);
	if (n)
	    putc(' ', fp);
    }
    putc('\n', fp);
}
#endif

/*
 * DUMP_CHECK - check that the dumps are not empty.
 */
#define DUMP_CHECK(D, NAME)						\
    if (D) { printf("->  %s is not empty:\n", NAME);			\
    writedump(env, D, stdout); }

/*
 * repl - read-eval-print loop.
 */
void repl(pEnv env)
{
    int ch;
    unsigned char flag;

    ch = getch(env);
    while (1) {
	ch = getsym(env, ch);
	if (env->sym == LIBRA || env->sym == HIDE || env->sym == MODULE_ ||
	    env->sym == CONST_) {
#ifdef NOBDW
	    inimem1(env, 1);	/* also resets the stack to initial */
#endif
	    if ((flag = env->sym == MODULE_) != 0)
		hide_inner_modules(env, 1);
	    ch = compound_def(env, ch);
	    if (flag)
		hide_inner_modules(env, 0);
#ifdef NOBDW
	    inimem2(env);	/* enlarge definition space */
#endif
	} else {
	    ch = readterm(env, ch);
	    if (env->stck && nodetype(env->stck) == LIST_) {
		env->prog = nodevalue(env->stck).lis;
		env->stck = nextnode1(env->stck);
#ifdef NOBDW
		env->conts = 0;
#endif
#ifdef BYTECODE
		if (env->bytecoding > 0) {
		    bytecode(env, env->prog);
		    return;
		}
#endif
		exeterm(env, env->prog);
	    }
#ifdef NOBDW
	    if (env->conts || env->dump || env->dump1 || env->dump2 ||
			      env->dump3 || env->dump4 || env->dump5) {
		printf("the dumps are not empty\n");
		DUMP_CHECK(env->conts, "conts");
		DUMP_CHECK(env->dump, "dump");
		DUMP_CHECK(env->dump1, "dump1");
		DUMP_CHECK(env->dump2, "dump2");
		DUMP_CHECK(env->dump3, "dump3");
		DUMP_CHECK(env->dump4, "dump4");
		DUMP_CHECK(env->dump5, "dump5");
	    }
#endif
	    print(env);
	}
    }
}
