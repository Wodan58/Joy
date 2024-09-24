/*
 *  module  : symbol.c
 *  version : 1.8
 *  date    : 09/16/24
 */
#include "globals.h"

/*
 * enterglobal stores an unknown global identifier in the symbol table.
 */
static int enterglobal(pEnv env, char *name)
{
    Entry ent;
    int index;

    index = vec_size(env->symtab);
    memset(&ent, 0, sizeof(ent));	/* make sure that all fields are 0 */
    ent.name = strdup(name);		/* copy to permanent memory */
    ent.is_user = 1;
    ent.flags = env->inlining ? IMMEDIATE : OK;
    ent.u.body = 0;			/* may be assigned in definition */
    addsymbol(env, ent, index);		/* add symbol entry to hash table */
    vec_push(env->symtab, ent);
    return index;
}

/*
 * Lookup first searches name in the local symbol tables, if not found in the
 * global symbol table, if still not found enters name in the global table.
 */
int lookup(pEnv env, char *name)
{
    int index;

    /*
     * Sequential search in the local tables is replaced by a search in the
     * hash table, where each entry receives a unique key built from module +
     * name, or section number + name, separated by a full stop. The hash table
     * thus contains local symbols, but each local section has its own numeric
     * identifier. Local symbols are only accessable from their accompanying
     * public sections. In case a qualified symbol cannot be found, it is
     * entered as classified symbol in the symbol table. Global symbols are not
     * added during the first time read of private sections.
     */
    if ((index = qualify(env, name)) == 0)
	/* not found, enter in global, unless it is a module-member */
	if (strchr(name, '.') == 0)
	    index = enterglobal(env, name);
    return index;
}

/*
 * Enteratom enters a symbol in the symbol table, maybe a local symbol. This
 * local symbol is also added to the hash table, but in its classified form.
 */
int enteratom(pEnv env, char *name)
{
    int index;

    /*
     * Local symbols are only added during the first read of private sections
     * and public sections of a module.
     * They should be found during the second read.
     */
    if ((index = qualify(env, name)) == 0)
	index = enterglobal(env, classify(env, name));
    return index;
}

/*
 * definition reads a definition. The right side is processed first. This makes
 * redefinition of "true" and "false" possible.
 */
static int definition(pEnv env, int ch)
{
    int index;
    Entry ent;
    char *name;

    if (env->sym == LIBRA || env->sym == PRIVATE || env->sym == HIDE
			  || env->sym == MODULE_ || env->sym == CONST_) {
	ch = compound_def(env, ch);
	if (env->sym == '.')
	    ch = getsym(env, ch);
	else
	    error("END or period '.' expected in compound definition");
	return ch;
    }

    if (env->sym != USR_)
	/*   NOW ALLOW EMPTY DEFINITION:
	      { error("atom expected at start of definition");
		abortexecution_(); }
	*/
	return ch;

    /* sym == USR_ : */
    name = GC_strdup(env->str);

    ch = getsym(env, ch);
    if (env->sym == EQDEF)
	ch = getsym(env, ch);
    else
	error("== expected in definition");
    ch = readterm(env, ch);

    index = enteratom(env, name);
    ent = vec_at(env->symtab, index);
    if (!ent.is_user && env->overwrite) {
	fflush(stdout);
	fprintf(stderr, "warning: overwriting inbuilt '%s'\n", ent.name);
    }
    ent.is_user = 1;
    ent.u.body = nodevalue(env->stck).lis;
    vec_at(env->symtab, index) = ent;
    env->stck = nextnode1(env->stck);
    return ch;
}

/*
 * defsequence reads a definition sequence. The definitions are stored in the
 * symbol table.
 */
static int defsequence(pEnv env, int ch)
{
    if (env->sym == CONST_)
	env->inlining = 1;
    do {
	ch = getsym(env, ch);
	ch = definition(env, ch);
    } while (env->sym == ';');
    env->inlining = 0;
    return ch;
}

/*
 * Handle a compound definition.
 */
int compound_def(pEnv env, int ch)
{
    switch (env->sym) {
    case MODULE_:
	ch = getsym(env, ch);
	if (env->sym != USR_)
	    abortexecution_(ABORT_RETRY);
	initmod(env, env->str);	/* initmod adds name to the module */
	ch = getsym(env, ch);
	ch = compound_def(env, ch);
	exitmod();		/* exitmod deregisters a module */
	break;

    case PRIVATE:
    case HIDE:
	initpriv(env);		/* initpriv increases the hide number */
	ch = defsequence(env, ch);
	stoppriv();		/* stoppriv changes private to public */
	ch = compound_def(env, ch);
	exitpriv();		/* exitpriv lowers the hide stack */
	break;

    case PUBLIC:
    case LIBRA:
    case IN__:
    case CONST_:
	ch = defsequence(env, ch);
	break;

    default:
	fflush(stdout);
	fprintf(stderr, "warning: empty compound definition\n");
	break;
    }
    return ch;
}
