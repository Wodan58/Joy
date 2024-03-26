/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.81
 *  date    : 03/21/24
 */

/*
07-May-03 condnestrec
17-Mar-03 modules
04-Dec-02 argc, argv
04-Nov-02 undefs
03-Apr-02 # comments
01-Feb-02 case

30-Oct-01 Fixed Bugs in file interp.c :

   1. division (/) by zero when one or both operands are floats
   2. fremove and frename gave wrong truth value
      (now success  => true)
   3. nullary, unary, binary, ternary had two bugs:
      a. coredump when there was nothing to push
	 e.g.   11 22 [pop pop] nullary
      b. produced circular ("infinite") list when no
	 new node had been created
	 e.g.   11 22 [pop] nullary
   4. app4 combinator was wrong.
      Also renamed:  app2 -> unary2,  app3 -> unary3, app4 -> unary4
      the old names can still be used, but are declared obsolete.
   5. Small additions to (raw) Joy:
      a)  putchars - previously defined in library file inilib.joy
      b) fputchars (analogous, for specified file)
	 fputstring (== fputchars for Heiko Kuhrt's program)
*/
#include "globals.h"

#define POP(X) X = nextnode1(X)

static void writestack(pEnv env, Index n)
{
    if (n) {
	writestack(env, nextnode1(n));
	if (nextnode1(n))
	    putchar(' ');
	writefactor(env, n, stdout);
    }
}

/*
 * exeterm starts with n. If during execution n comes up again, the function
 * is directly recursive. That is allowed, except when the very first factor
 * is n. In that case there is recursion without an end condition.
 * It is possible to discover that specific case.
 */
void exeterm(pEnv env, Index n)
{
    Entry ent;
    int type, index;
    Index stepper, root = 0;

start:
    env->calls++;
    if (root == n)
	return;
    root = n;
#ifdef NOBDW
    env->bucket.lis = n;
    env->conts = newnode(env, LIST_, env->bucket, env->conts);
    while (nodevalue(env->conts).lis) {
#else
    while (n) {
#endif
#ifdef TRACEGC
	if (env->tracegc > 5) {
	    printf("exeterm1: ");
	    printnode(env, nodevalue(env->conts).lis);
	}
#endif
#ifdef NOBDW
	stepper = nodevalue(env->conts).lis;
	nodevalue(env->conts).lis = nextnode1(nodevalue(env->conts).lis);
#else
	stepper = n;
#endif
	env->opers++;
	if (env->debugging) {
	    writestack(env, env->stck);
	    if (env->debugging == 2) {
		printf(" : ");
		writeterm(env, stepper, stdout);
	    }
	    putchar('\n');
	}
	type = nodetype(stepper);
	switch (type) {
	case ILLEGAL_:
	case COPIED_:
	    fflush(stdout);
	    fprintf(stderr, "exeterm: attempting to execute bad node\n");
#ifdef TRACEGC
	    printnode(env, stepper);
#endif
	    return;
	case USR_:
	    index = nodevalue(stepper).ent;
	    ent = vec_at(env->symtab, index);
	    if (!ent.u.body) {
		if (env->undeferror)
		    execerror("definition", ent.name);
#ifdef NOBDW
		continue;
#else
		break;
#endif
	    }
	    if (!nextnode1(stepper)) {
#ifdef NOBDW
		POP(env->conts);
#endif
		n = ent.u.body;
		goto start;
	    }
	    if (ent.u.body != root)
		exeterm(env, ent.u.body);
	    break;

	case ANON_FUNCT_:
	    (*nodevalue(stepper).proc)(env);
	    break;

	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	case STRING_:
	case LIST_:
	case FLOAT_:
	case FILE_:
	    env->stck = newnode(env, type, nodevalue(stepper), env->stck);
	    break;

	default:
	    execerror("valid factor", "exeterm");
	    break;
	}
#ifdef TRACEGC
	if (env->tracegc > 5) {
	    printf("exeterm2: ");
	    printnode(env, stepper);
	}
#endif
#ifndef NOBDW
	n = n->next;
#endif
    }
#ifdef NOBDW
    POP(env->conts);
#endif
}
/* END of INTERP.C */
