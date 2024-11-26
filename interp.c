/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.87
 *  date    : 11/15/24
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
#include "builtin.h"

static void writestack(pEnv env, Index n)
{
    if (n) {
	writestack(env, nextnode1(n));
	if (nextnode1(n))
	    putchar(' ');
	writefactor(env, n, stdout);
    }
}

#ifdef COMPILER
int count_quot(pEnv env)
{
    Node *node;
    int count = 0;

    for (node = env->stck; node; node = node->next)
	if (node->op == LIST_)
	    count++;
	else
	    break;
    return count;
}

int is_valid_C_identifier(char *str)
{
    if (*str != '_' && !isalpha((int)*str))
	return 0;
    for (str++; *str; str++)
	if (*str != '_' && !isalnum((int)*str))
	    return 0;
    return 1;
}
#endif

/*
 * exeterm evaluates a sequence of factors. There is no protection against
 * recursion without end condition: it will overflow the call stack.
 */
void exeterm(pEnv env, Index n)
{
    Index p;
    int index;
    Entry ent;
#ifdef COMPILER
    const char *ptr;
    int leng, nofun;
#endif

start:
    env->calls++;
    if (!n)
	return;					/* skip empty program */
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
		    execerror(env, "definition", ent.name);
#ifdef NOBDW
		continue;		/* skip empty body */
#else
		break;
#endif
	    }
#ifdef COMPILER
	    if (env->compiling > 0) {
		/*
		 * Functions are inlined unless they are called recursively.
		 */
		if ((ent.cflags & IS_ACTIVE) == 0) {
		    /*
		     * The ACTIVE flag prevents endless recursion.
		     */
		    ent.cflags |= IS_ACTIVE;
		    vec_at(env->symtab, index) = ent;
		    exeterm(env, ent.u.body);
		    /*
		     * Update symbol table, but first reread the entry.
		     */
		    ent = vec_at(env->symtab, index);
		    ent.cflags &= ~IS_ACTIVE;
		    vec_at(env->symtab, index) = ent;
		    break;
		}
		/*
		 * A recursive function can be printed as is, if the name is
		 * a valid C identifier and otherwise it is made into one.
		 */
		if (!is_valid_C_identifier(ent.name)) {
		    ptr = identifier(ent.name);
		    leng = strlen(ptr) + 4;
		    ent.name = GC_malloc_atomic(leng);
		    snprintf(ent.name, leng, "do_%s", ptr);
		}
		/*
		 * The USED flag causes the function to be printed.
		 */
		ent.cflags |= IS_USED;
		vec_at(env->symtab, index) = ent;
		printstack(env);
		fprintf(env->outfp, "%s(env);\n", ent.name);
		break;
	    }
#endif	    
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
#ifdef COMPILER
	    if (env->compiling > 0) {
		index = operindex(env, nodevalue(p).proc);
		ent = vec_at(env->symtab, index);
		/*
		 * Functions that have a template are filled with the
		 * quotations they need. When compiling for Soy, the templates
		 * are not used. The nofun flag is also set when the template
		 * file was not found.
		 */
		nofun = env->compiling == 3;
		if (!nofun && ent.qcode && ent.qcode <= count_quot(env)) {
		    if (instance(env, ent.name, ent.qcode))
			break;
		    nofun = 1;
		}
		/*
		 * Functions are flagged as used, even when they are
		 * evaluated at compile time.
		 */
		ent = vec_at(env->symtab, index);
		ent.cflags |= IS_USED;
		vec_at(env->symtab, index) = ent;
		/*
		 * An exception needs to be made for dup_ in case the stack
		 * contains a list.
		 */
		if (env->stck && nodetype(env->stck) == LIST_ &&
				ent.u.proc == dup_)
		    nofun = 1;
		/*
		 * Functions that cannot be evaluated at compile time
		 * are sent to output. There is no need for a nickname.
		 */
		if (nofun || ent.nofun) {
		    printstack(env);
		    fprintf(env->outfp, "%s_(env);\n", ent.name);
		    break;
		}
	    }
#endif	    
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
	    GNULLARY(p);
	    break;
	default:
	    execerror(env, "valid factor", "exeterm");
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
