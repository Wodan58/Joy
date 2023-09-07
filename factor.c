/* FILE: factor.c */
/*
 *  module  : factor.c
 *  version : 1.22
 *  date    : 09/07/23
 */
#include "globals.h"

/*
    readfactor - read a factor from srcfile and push it on the stack.
		 In case of LPAREN nothing gets pushed.
*/
PUBLIC void readfactor(pEnv env) /* read a JOY factor */
{
    Entry ent;
    uint64_t set = 0;

    switch (env->symb) {
    case ATOM:
	lookup(env);
	if (!env->location && strchr(env->yylval.str, '.')) {
	    error(env, "no such field in module");
	    return;
	}
	ent = vec_at(env->symtab, env->location);
	/* execute immediate functions at compile time */
	if (ent.flags == IMMEDIATE)
	    (*ent.u.proc)(env);
	else if (ent.is_user) {
	    env->bucket.ent = env->location;
	    env->stck = newnode(env, USR_, env->bucket, env->stck);
	} else {
	    env->yylval.proc = ent.u.proc;
	    env->stck = newnode(env, ANON_FUNCT_, env->yylval, env->stck);
	}
	return;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
    case STRING_:
    case FLOAT_:
	env->stck = newnode(env, env->symb, env->yylval, env->stck);
	return;
    case LBRACE:
	while (getsym(env), env->symb <= ATOM)
	    if ((env->symb != CHAR_ && env->symb != INTEGER_)
		|| env->yylval.num < 0 || env->yylval.num >= SETSIZE)
		error(env, "small numeric expected in set");
	    else
		set |= ((int64_t)1 << env->yylval.num);
	env->bucket.set = set;
	env->stck = newnode(env, SET_, env->bucket, env->stck);
	if (env->symb != RBRACE)
	    error(env, "'}' expected");
	return;
    case LBRACK:
	getsym(env);
	readterm(env);
	if (env->symb != RBRACK)
	    error(env, "']' expected");
	return;
    case LPAREN:
	error(env, "'(' not implemented");
	getsym(env);
	return;
    default:
	error(env, "a factor cannot begin with this symbol");
    }
}

/*
    readterm - read a term from srcfile and push this on the stack as a list.
*/
#ifdef NOBDW
PUBLIC void readterm(pEnv env)
{
    env->bucket.lis = 0;
    env->stck = newnode(env, LIST_, env->bucket, env->stck);
    if (env->symb <= ATOM) {
	readfactor(env);
	if (env->stck) {
	    nodevalue(nextnode1(env->stck)).lis = env->stck;
	    env->stck = nextnode1(env->stck);
	    nextnode1(nodevalue(env->stck).lis) = 0;
	    env->dump = newnode(env, LIST_, nodevalue(env->stck), env->dump);
	}
	while (getsym(env), env->symb <= ATOM) {
	    readfactor(env);
	    if (env->stck) {
		nextnode1(nodevalue(env->dump).lis) = env->stck;
		env->stck = nextnode1(env->stck);
		nextnode2(nodevalue(env->dump).lis) = 0;
		nodevalue(env->dump).lis = nextnode1(nodevalue(env->dump).lis);
	    }
	}
	env->dump = nextnode1(env->dump);
    }
}
#else
/*
    readterm - read a term from srcfile and push this on the stack as a list.
*/
PUBLIC void readterm(pEnv env)
{
    Index *dump = 0;

    env->bucket.lis = 0;
    env->stck = newnode(env, LIST_, env->bucket, env->stck);
    dump = &nodevalue(env->stck).lis;
    while (env->symb <= ATOM) {
	readfactor(env);
	if (env->stck) {
	    *dump = env->stck;
	    dump = &nextnode1(env->stck);
	    env->stck = *dump;
	    *dump = 0;
	}
	getsym(env);
    }
}
#endif

/*
    writefactor - print a factor in readable format to stdout.
*/
PUBLIC void writefactor(pEnv env, Index n, FILE *fp)
{
    int i;
    char *p;
    uint64_t set;

/*
    This cannot happen. Factor has a small number of customers: writeterm,
    main, put, fput. They all check that the stack is not empty, so this code
    only serves as a reminder for future customers.
*/
#if 0
    if (!n)
	execerror("non-empty stack", "print");
#endif
    switch (nodetype(n)) {
    case USR_:
	fprintf(fp, "%s", vec_at(env->symtab, nodevalue(n).ent).name);
	return;
    case ANON_FUNCT_:
	fprintf(fp, "%s", opername(operindex(nodevalue(n).proc)));
	return;
    case BOOLEAN_:
	fprintf(fp, "%s", nodevalue(n).num ? "true" : "false");
	return;
    case CHAR_:
	if (nodevalue(n).num >= 8 && nodevalue(n).num <= 13)
	    fprintf(fp, "'\\%c", "btnvfr"[nodevalue(n).num - 8]);
	else
	    fprintf(fp, "'%c", (int)nodevalue(n).num);
	return;
    case UNKNOWN_:
    case INTEGER_:
	fprintf(fp, "%" PRId64, nodevalue(n).num);
	return;
    case SET_:
	putc('{', fp);
	for (i = 0, set = nodevalue(n).set; i < SETSIZE; i++)
	    if (set & ((int64_t)1 << i)) {
		fprintf(fp, "%d", i);
		set &= ~((int64_t)1 << i);
		if (set)
		    putc(' ', fp);
	    }
	putc('}', fp);
	return;
    case STRING_:
	putc('"', fp);
	for (p = nodevalue(n).str; *p; p++)
	    if (*p >= 8 && *p <= 13)
		fprintf(fp, "\\%c", "btnvfr"[*p - 8]);
	    else
		putc(*p, fp);
	putc('"', fp);
	return;
    case LIST_:
	putc('[', fp);
	writeterm(env, nodevalue(n).lis, fp);
	putc(']', fp);
	return;
    case FLOAT_:
	fprintf(fp, "%g", nodevalue(n).dbl);
	return;
    case FILE_:
	if (!nodevalue(n).fil)
	    fprintf(fp, "file:NULL");
	else if (nodevalue(n).fil == stdin)
	    fprintf(fp, "file:stdin");
	else if (nodevalue(n).fil == stdout)
	    fprintf(fp, "file:stdout");
	else if (nodevalue(n).fil == stderr)
	    fprintf(fp, "file:stderr");
	else
	    fprintf(fp, "file:%p", nodevalue(n).fil);
	return;
    default:
	error(env, "a factor cannot begin with this symbol");
    }
}

/*
    writeterm - print the contents of a list in readable format to stdout.
*/
PUBLIC void writeterm(pEnv env, Index n, FILE *fp)
{
    while (n) {
	writefactor(env, n, fp);
	if ((n = nextnode1(n)) != 0)
	    putc(' ', fp);
    }
}

/*
    writedump - print the contents of a dump in readable format to stdout,
		thereby limiting the write to 10 factors. It is possible that
		the dump contains circular references.
*/
#ifdef NOBDW
PUBLIC void writedump(pEnv env, Index n, FILE *fp)
{
    int i = 0;

    for (i = 0; n && i < 10; i++) {
	writefactor(env, n, fp);
	if ((n = nextnode1(n)) != 0)
	    putc(' ', fp);
    }
}
#endif
