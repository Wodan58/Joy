/* FILE: interp.c */
/*
 *  module  : interp.c
 *  version : 1.92
 *  date    : 02/09/26
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

/*
 * exeterm evaluates a sequence of factors. There is no protection against
 * recursion without end condition: it will overflow the call stack (even
 * if the call stack is set to an unreasonable maximum, like 1 GiB.)
 */
#ifdef NOBDW
void exeterm(pEnv env, Index n)
{
    int index;
    Entry ent;
start:
    env->calls++;
    if (!n)
	return;					/* skip empty program */
    env->conts = LIST_NEWNODE(n, env->conts);	/* root for garbage collector */
    while (nodevalue(env->conts).lis) {
	n = nodevalue(env->conts).lis;
	POP(nodevalue(env->conts).lis);
	env->opers++;
	if (env->debugging) {
	    writestack(env, env->stck);		/* top of stack on the right */
	    if (env->debugging == 2) {
		printf(" : ");
		writeterm(env, n, stdout);
	    }
	    putchar('\n');
	    fflush(stdout);
	}
	switch (nodetype(n)) {
	case ILLEGAL_:
	case COPIED_:
	    fflush(stdout);
	    fprintf(stderr, "exeterm: attempting to execute bad node\n");
	    return;
	case USR_:
	    index = nodevalue(n).ent;
	    ent = vec_at(env->symtab, index);
	    if (!ent.u.body) {
		if (env->undeferror)
		    execerror(env, "definition", ent.name);
		continue;		/* skip empty body */
	    }
	    if (!nextnode1(n)) {
		POP(env->conts);
		n = ent.u.body;
		goto start;		/* tail call optimization */
	    }
	    exeterm(env, ent.u.body);	/* subroutine call */
	    break;
	case ANON_FUNCT_:
	    (*nodevalue(n).proc)(env);
	    break;
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	case STRING_:
	case LIST_:
	case FLOAT_:
	case FILE_:
	    GNULLARY(n);
	    break;
	default:
	    execerror(env, "valid factor", "exeterm");
	}
    }
    POP(env->conts);
}
#else
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

/* Illegal opcode execution request */
void do_illegal(pEnv env)
{
    fflush(stdout);
    fprintf(stderr, "exeterm: attempting to execute bad node\n");
    execerror(env, "valid opcode", "exeterm");
}	/* LCOV_EXCL_LINE */

/* Evaluate a user defined library function */
void do_lib(pEnv env)
{
    int index;
    Entry ent;

    index = nodevalue(env->conts).ent;
    ent = vec_at(env->symtab, index);
    if (!ent.u.body && env->undeferror)
	execerror(env, "definition", ent.name);
    exeterm(env, ent.u.body);	/* subroutine call */
}

/* Anonymous function call */
void do_proc(pEnv env)
{
    (*nodevalue(env->conts).proc)(env);
}

/* BOOLEAN_, CHAR_, INTEGER_, ... */
void do_data(pEnv env)
{
    GNULLARY(env->conts);
}

/* Genuine Nybble Code Interpreter */
static proc_t opcodes[] = {
    do_illegal,
    do_illegal,
    do_lib,
    do_proc,
    do_data,	/* BOOLEAN */
    do_data,	/* CHAR */
    do_data,	/* INTEGER */
    do_data,	/* SET */
    do_data,	/* STRING */
    do_data,	/* LIST */
    do_data,	/* FLOAT */
    do_data,	/* FILE */
    do_data,	/* BIGNUM */
    id_,
    id_,
    id_
};

void exeterm(pEnv env, Index n)
{
#ifdef COMPILER
#ifdef INLINING
    int leng;
#endif
    int index;
    Entry ent;
    int nofun;
    const char *ptr;
start:
#endif
    env->calls++;
    for (; n; POP(n)) {				/* skip empty program */
	env->opers++;
	if (env->debugging) {
	    writestack(env, env->stck);		/* top of stack on the right */
	    if (env->debugging == 2) {
		printf(" : ");
		writeterm(env, n, stdout);
	    }
	    putchar('\n');
	    fflush(stdout);
	}
#ifndef COMPILER
	env->conts = n;
	(*opcodes[nodetype(n)])(env);		/* nybble code interpreter */
#else
	/*
	 * The switch on nodetype is now only used by the compiler.
	 */
	switch (nodetype(n)) {
	case ILLEGAL_:
	case COPIED_:
	    fflush(stdout);
	    fprintf(stderr, "exeterm: attempting to execute bad node\n");
	    return;
	case USR_:
	    index = nodevalue(n).ent;
	    ent = vec_at(env->symtab, index);
	    if (!ent.u.body) {
		if (env->undeferror)
		    execerror(env, "definition", ent.name);
		break;
	    }
	    if (env->compiling > 0) {
		/*
		 * Functions are inlined unless they are called recursively.
		 */
#ifdef INLINING
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
		    snprintf(ent.name, leng, "%s", ptr);
		}
#endif
		/*
		 * The USED flag causes the function to be printed.
		 */
		ent.cflags |= IS_USED;
		vec_at(env->symtab, index) = ent;
		printstack(env);
		if (index < tablesize())
		    fprintf(env->outfp, "%s_(env);\n", nickname(index));
		else
		    fprintf(env->outfp, "do_%s(env);\n", ent.name);
		break;
	    }
	    if (!nextnode1(n)) {
		n = ent.u.body;
		goto start;		/* tail call optimization */
	    }
	    exeterm(env, ent.u.body);	/* subroutine call */
	    break;
	case ANON_FUNCT_:
	    if (env->compiling > 0) {
		index = operindex(env, nodevalue(n).proc);
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
		    ptr = index < tablesize() ? nickname(index) : ent.name;
		    fprintf(env->outfp, "%s_(env);\n", ptr);
		    break;
		}
	    }
	    (*nodevalue(n).proc)(env);
	    break;
	case BOOLEAN_:
	case CHAR_:
	case INTEGER_:
	case SET_:
	case STRING_:
	case LIST_:
	case FLOAT_:
	case FILE_:
	    GNULLARY(n);
	    break;

	default:
	    execerror(env, "valid factor", "exeterm");
	}
#endif
    }
}
#endif
/* END of INTERP.C */
