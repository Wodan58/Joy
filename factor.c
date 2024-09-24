/*
 *  module  : factor.c
 *  version : 1.35
 *  date    : 09/16/24
 */
#include "globals.h"

/*
 * list2set converts a list to a set.
 */
static uint64_t list2set(pEnv env, Index n)
{
    uint64_t set;

    for (set = 0; n; POP(n))
	switch (nodetype(n)) {
	case CHAR_:
	case INTEGER_:
	    if (nodevalue(n).num < 0 || nodevalue(n).num >= SETSIZE)
		error("small numeric expected in set");
	    else
		set |= (uint64_t)1 << nodevalue(n).set;
	    break;
	default:
	    error("numeric expected in set");
	    break;
	}
    return set;
}

/*
 * readfactor - read a factor from srcfile and push it on the stack.
 * In case of an error nothing is pushed on the stack and rv is set to 0.
 */
int readfactor(pEnv env, int ch, int *rv)	/* read a JOY factor */
{
    int index;
    Entry ent;
    uint64_t set;

    *rv = 1;	/* assume that a factor will be read */
    switch (env->sym) {
    case USR_:
	if ((index = lookup(env, env->str)) == 0) {
	    error("no such field in module");
	    *rv = 0;	/* no factor was read */
	    break;
	}
	ent = vec_at(env->symtab, index);
	/* execute immediate functions at compile time */
	if (ent.flags == IMMEDIATE) {
	    if (ent.is_user)
		exeterm(env, ent.u.body);
	    else
		(*ent.u.proc)(env);
	} else if (ent.is_user)
	    NULLARY(USR_NEWNODE, index);
	else
	    NULLARY(ANON_FUNCT_NEWNODE, ent.u.proc);
	break;

#if 0
    /* A boolean is no longer returned by the scanner */
    case BOOLEAN_:
	NULLARY(BOOLEAN_NEWNODE, env->num);
	break;
#endif

    case CHAR_:
	NULLARY(CHAR_NEWNODE, env->num);
	break;

    case INTEGER_:
	NULLARY(INTEGER_NEWNODE, env->num);
	break;

    case STRING_:
	NULLARY(STRING_NEWNODE, env->str);
	break;

    case FLOAT_:
	NULLARY(FLOAT_NEWNODE, env->dbl);
	break;

    case '{':
	ch = getsym(env, ch);
	ch = readterm(env, ch);
	set = list2set(env, nodevalue(env->stck).lis);
	UNARY(SET_NEWNODE, set);
	if (env->sym != '}')
	    error("'}' expected");
	break;

    case '[':
	ch = getsym(env, ch);
	ch = readterm(env, ch);
	if (env->sym != ']')
	    error("']' expected");
	break;

    case '(':
	error("'(' not implemented");
	*rv = 0;	/* no factor was read */
	break;

    default:
	error("a factor cannot begin with this symbol");
	*rv = 0;	/* no factor was read */
	break;
    }
    return ch;
}

/*
 * readterm - read a term from srcfile and push this on the stack as a list.
 */
#ifdef NOBDW
int readterm(pEnv env, int ch)
{
    int rv = 0, first = 1;

    NULLARY(LIST_NEWNODE, 0);
    while (1) {
	if (strchr(".;]}", env->sym) ||
			  (env->sym >= LIBRA && env->sym <= CONST_))
	    break;
	ch = readfactor(env, ch, &rv);
	if (rv) {
	    if (first) {
		first = 0;
		nodevalue(nextnode1(env->stck)).lis = env->stck;
		POP(env->stck);
		nextnode1(nodevalue(env->stck).lis) = 0;
		env->dump = LIST_NEWNODE(nodevalue(env->stck).lis, env->dump);
	    } else {
		nextnode1(nodevalue(env->dump).lis) = env->stck;
		POP(env->stck);
		nextnode2(nodevalue(env->dump).lis) = 0;
		nodevalue(env->dump).lis = nextnode1(nodevalue(env->dump).lis);
	    }
	}
	ch = getsym(env, ch);
    }
    if (!first)
	POP(env->dump);
    return ch;
}
#else
/*
 * readterm - read a term from srcfile and pushes this on the stack as a list.
 */
int readterm(pEnv env, int ch)
{
    int rv = 0;
    Index *dump = 0;

    NULLARY(LIST_NEWNODE, 0);
    dump = &nodevalue(env->stck).lis;
    while (1) {
	if (strchr(".;]}", env->sym) ||
			  (env->sym >= LIBRA && env->sym <= CONST_))
	    break;
	ch = readfactor(env, ch, &rv);
	if (rv) {
	    *dump = env->stck;
	    dump = &nextnode1(env->stck);
	    env->stck = *dump;
	    *dump = 0;
	}
	ch = getsym(env, ch);
    }
    return ch;
}
#endif
