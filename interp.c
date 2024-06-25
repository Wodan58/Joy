/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.83
 *  date    : 06/24/24
 */

#if 0
#define TRACEGC
#endif

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
 * exeterm evaluates a sequence of factors. There is no protection against
 * recursion without end condition: it will overflow the call stack.
 */
void exeterm(pEnv env, Index n)
{
    Index p;
    int index;
    Entry ent;

start:
    env->calls++;
    if (!n)
	return;				/* skip empty program */
#ifdef NOBDW
    env->conts = LIST_NEWNODE(n, env->conts);	/* root for garbage collector */
    while (nodevalue(env->conts).lis) {
#else
    while (n) {
#endif
#ifdef TRACEGC
	if (env->tracegc > 5) {
	    printf("exeterm1: ");
	    printnode(env, n);
	}
#endif
#ifdef NOBDW
	p = nodevalue(env->conts).lis;
	POP(nodevalue(env->conts).lis);
#else
	p = n;
#endif
	env->opers++;
	if (env->debugging) {
	    writestack(env, env->stck);
	    if (env->debugging == 2) {
		printf(" : ");
		writeterm(env, p, stdout);
	    }
	    putchar('\n');
	    fflush(stdout);
	}
	switch (nodetype(p)) {
	case ILLEGAL_:
	case COPIED_:
	    fflush(stdout);
	    fprintf(stderr, "exeterm: attempting to execute bad node\n");
#ifdef TRACEGC
	    printnode(env, p);
#endif
	    return;
	case USR_:
	    index = nodevalue(p).ent;
	    ent = vec_at(env->symtab, index);
	    if (!ent.u.body) {
		if (env->undeferror)
		    execerror("definition", ent.name);
#ifdef NOBDW
		continue;	/* skip empty body */
#else
		break;
#endif
	    }
	    if (!nextnode1(p)) {
#ifdef NOBDW
		POP(env->conts);
#endif
		n = ent.u.body;
		goto start;		/* tail call optimization */
	    }
	    exeterm(env, ent.u.body);	/* subroutine call */
	    break;
	case ANON_FUNCT_:
	    (*nodevalue(p).proc)(env);
	    break;
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	case STRING_:
	case LIST_:
	case FLOAT_:
	case FILE_:
	    env->stck = newnode2(env, p, env->stck);
	    break;
	default:
	    execerror("valid factor", "exeterm");
	    break;
	}
#ifdef TRACEGC
	if (env->tracegc > 5) {
	    printf("exeterm2: ");
	    printnode(env, p);
	}
#endif
#ifndef NOBDW
	POP(n);
#endif
    }
#ifdef NOBDW
    POP(env->conts);
#endif
}
/* END of INTERP.C */
