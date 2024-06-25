/* FILE: factor.c */
/*
 *  module  : factor.c
 *  version : 1.33
 *  date    : 06/24/24
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
		set |= (uint64_t)1 << nodevalue(n).num;
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

/*
 * writefactor - print a factor in readable format to stdout.
 */
void writefactor(pEnv env, Index n, FILE *fp)
{
    int i;
    uint64_t set, j;
    char *ptr, buf[BUFFERMAX], tmp[MAXNUM];

#if 0
/*
 * This cannot happen. writefactor has a small number of customers: writeterm,
 * main, put, fput. They all check that the stack is not empty, so this code
 * only serves as a reminder for future customers.
 */
    if (!n)
	execerror("non-empty stack", "writefactor");
#endif
    switch (nodetype(n)) {
    case USR_:
	fprintf(fp, "%s", vec_at(env->symtab, nodevalue(n).ent).name);
	break;

    case ANON_FUNCT_:
	fprintf(fp, "%s", opername(operindex(env, nodevalue(n).proc)));
	break;

    case BOOLEAN_:
	fprintf(fp, "%s", nodevalue(n).num ? "true" : "false");
	break;

    case CHAR_:
	if (nodevalue(n).num >= 8 && nodevalue(n).num <= 13)
	    fprintf(fp, "'\\%c", "btnvfr"[nodevalue(n).num - 8]);
	else if (iscntrl(nodevalue(n).num) || nodevalue(n).num == 32)
	    fprintf(fp, "'\\%03d", (int)nodevalue(n).num);
	else
	    fprintf(fp, "'%c", (int)nodevalue(n).num);
	break;

    case INTEGER_:
	fprintf(fp, "%" PRId64, nodevalue(n).num);
	break;

    case SET_:
	putc('{', fp);
	for (i = 0, j = 1, set = nodevalue(n).set; i < SETSIZE; i++, j <<= 1)
	    if (set & j) {
		fprintf(fp, "%d", i);
		set &= ~j;
		if (set)
		    putc(' ', fp);
	    }
	putc('}', fp);
	break;

    case STRING_:
	putc('"', fp);
#ifdef NOBDW
	for (ptr = (char *)&nodevalue(n); *ptr; ptr++)
#else
	for (ptr = nodevalue(n).str; *ptr; ptr++)
#endif
	    if (*ptr == '"')
		fprintf(fp, "\\\"");
	    else if (*ptr >= 8 && *ptr <= 13)
		fprintf(fp, "\\%c", "btnvfr"[*ptr - 8]);
	    else if (iscntrl((int)*ptr))
		fprintf(fp, "\\%03d", *ptr);
	    else
		putc(*ptr, fp);
	putc('"', fp);
	break;

    case LIST_:
	putc('[', fp);
	writeterm(env, nodevalue(n).lis, fp);
	putc(']', fp);
	break;

    case FLOAT_:
	sprintf(buf, "%g", nodevalue(n).dbl);	/* exponent character is e */
	if ((ptr = strchr(buf, '.')) == 0) {	/* locate decimal point */
	    if ((ptr = strchr(buf, 'e')) == 0) {/* locate start of exponent */
		i = buf[strlen(buf) - 1];
		if (isdigit(i))			/* check digit present */
		    strcat(buf, ".0");		/* add decimal point and 0 */
	    } else {
		strcpy(tmp, ptr);		/* save exponent */
		sprintf(ptr, ".0%s", tmp);	/* insert decimal point + 0 */
	    }
	}
	fprintf(fp, "%s", buf);
	break;

    case FILE_:
	if (!nodevalue(n).fil)
	    fprintf(fp, "NULL");
	else if (nodevalue(n).fil == stdin)
	    fprintf(fp, "stdin");
	else if (nodevalue(n).fil == stdout)
	    fprintf(fp, "stdout");
	else if (nodevalue(n).fil == stderr)
	    fprintf(fp, "stderr");
	else
	    fprintf(fp, "%p", (void *)nodevalue(n).fil);
	break;

    case BIGNUM_:
#ifdef NOBDW
	fprintf(fp, "%s", (char *)&nodevalue(n));
#else
	fprintf(fp, "%s", nodevalue(n).str);
#endif
	break;

    default:
	error("a factor cannot begin with this symbol");
	break;
    }
}

/*
 * writeterm - print the contents of a list in readable format to fp.
 */
void writeterm(pEnv env, Index n, FILE *fp)
{
    while (n) {
	writefactor(env, n, fp);
	POP(n);
	if (n)
	    putc(' ', fp);
    }
}

/*
 * writedump - print the contents of a dump in readable format to fp,
 *	       with a limit of 10 factors.
 */
#ifdef NOBDW
void writedump(pEnv env, Index n, FILE *fp)
{
    int i;

    for (i = 0; n && i < 10; i++) {
	writefactor(env, n, fp);
	POP(n);
	if (n)
	    putc(' ', fp);
    }
}
#endif
