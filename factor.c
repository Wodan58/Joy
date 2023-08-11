/* FILE: factor.c */
/*
 *  module  : factor.c
 *  version : 1.18
 *  date    : 08/11/23
 */
#include "globals.h"

/*
    readfactor - read a factor from srcfile and push it on the stack.
		 In case of LPAREN nothing gets pushed.
*/
PUBLIC void readfactor(pEnv env, int priv) /* read a JOY factor */
{
    Entry ent;
    uint64_t set = 0;

    switch (env->symb) {
    case ATOM:
	if (!priv) {
	    lookup(env);
	    if (!env->location && strchr(env->yylval.str, '.')) {
		error(env, "no such field in module");
		return;
	    }
	    ent = vec_at(env->symtab, env->location);
	    /* execute immediate functions at compile time */
	    if (env->location >= FALSE_ && env->location <= MAXINT_)
		(*ent.u.proc)(env);
	    else if (!ent.is_user) {
		env->yylval.proc = ent.u.proc;
		env->stck = newnode(env, env->location, env->yylval, env->stck);
	    } else {
		env->bucket.ent = env->location;
		env->stck = newnode(env, USR_, env->bucket, env->stck);
	    }
	}
	return;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
    case STRING_:
    case FLOAT_:
	if (!priv)
	    env->stck = newnode(env, env->symb, env->yylval, env->stck);
	return;
    case LBRACE:
	while (getsym(env), env->symb <= ATOM)
	    if ((env->symb != CHAR_ && env->symb != INTEGER_)
		|| env->yylval.num < 0 || env->yylval.num >= SETSIZE)
		error(env, "small numeric expected in set");
	    else
		set |= ((int64_t)1 << env->yylval.num);
	if (!priv) {
	    env->bucket.set = set;
	    env->stck = newnode(env, SET_, env->bucket, env->stck);
	}
	if (env->symb != RBRACE)
	    error(env, "'}' expected");
	return;
    case LBRACK:
	getsym(env);
	readterm(env, priv);
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
PUBLIC void readterm(pEnv env, int priv)
{
    if (!priv) {
	env->bucket.lis = 0;
	env->stck = newnode(env, LIST_, env->bucket, env->stck);
    }
    if (env->symb <= ATOM) {
	readfactor(env, priv);
	if (!priv && env->stck) {
	    nodevalue(nextnode1(env->stck)).lis = env->stck;
	    env->stck = nextnode1(env->stck);
	    nextnode1(nodevalue(env->stck).lis) = 0;
	    env->dump = newnode(env, LIST_, nodevalue(env->stck), env->dump);
	}
	while (getsym(env), env->symb <= ATOM) {
	    readfactor(env, priv);
	    if (!priv && env->stck) {
		nextnode1(nodevalue(env->dump).lis) = env->stck;
		env->stck = nextnode1(env->stck);
		nextnode2(nodevalue(env->dump).lis) = 0;
		nodevalue(env->dump).lis = nextnode1(nodevalue(env->dump).lis);
	    }
	}
	if (!priv)
	    env->dump = nextnode1(env->dump);
    }
}
#else
/*
    readterm - read a term from srcfile and push this on the stack as a list.
*/
PUBLIC void readterm(pEnv env, int priv)
{
    Index *dump = 0;

    if (!priv) {
	env->bucket.lis = 0;
	env->stck = newnode(env, LIST_, env->bucket, env->stck);
	dump = &nodevalue(env->stck).lis;
    }
    while (env->symb <= ATOM) {
	readfactor(env, priv);
	if (!priv && env->stck) {
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
PUBLIC void writefactor(pEnv env, Index n)
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
    switch (opertype(nodetype(n))) {
    case USR_:
	printf("%s", vec_at(env->symtab, nodevalue(n).ent).name);
	return;
    case BOOLEAN_:
	printf("%s", nodevalue(n).num ? "true" : "false");
	return;
    case CHAR_:
	if (nodevalue(n).num >= 8 && nodevalue(n).num <= 13)
	    printf("'\\%c", "btnvfr"[nodevalue(n).num - 8]);
	else
	    printf("'%c", (int)nodevalue(n).num);
	return;
    case INTEGER_:
	printf("%" PRId64, nodevalue(n).num);
	return;
    case SET_:
	putchar('{');
	for (i = 0, set = nodevalue(n).set; i < SETSIZE; i++)
	    if (set & ((int64_t)1 << i)) {
		printf("%d", i);
		set &= ~((int64_t)1 << i);
		if (set)
		    putchar(' ');
	    }
	putchar('}');
	return;
    case STRING_:
	putchar('"');
	for (p = nodevalue(n).str; *p; p++)
	    if (*p >= 8 && *p <= 13)
		printf("\\%c", "btnvfr"[*p - 8]);
	    else
		putchar(*p);
	putchar('"');
	return;
    case LIST_:
	putchar('[');
	writeterm(env, nodevalue(n).lis);
	putchar(']');
	return;
    case FLOAT_:
	printf("%g", nodevalue(n).dbl);
	return;
    case FILE_:
	if (!nodevalue(n).fil)
	    printf("file:NULL");
	else if (nodevalue(n).fil == stdin)
	    printf("file:stdin");
	else if (nodevalue(n).fil == stdout)
	    printf("file:stdout");
	else if (nodevalue(n).fil == stderr)
	    printf("file:stderr");
	else
	    printf("file:%p", nodevalue(n).fil);
	return;
    default:
	printf("%s", opername(nodetype(n)));
	return;
    }
}

/*
    writeterm - print the contents of a list in readable format to stdout.
*/
PUBLIC void writeterm(pEnv env, Index n)
{
    while (n) {
	writefactor(env, n);
	if ((n = nextnode1(n)) != 0)
	    putchar(' ');
    }
}

/*
    writedump - print the contents of a dump in readable format to stdout,
		thereby limiting the write to 10 factors. It is possible that
		the dump contains circular references.
*/
#ifdef NOBDW
PUBLIC void writedump(pEnv env, Index n)
{
    int i = 0;

    for (i = 0; n && i < 10; i++) {
	writefactor(env, n);
	if ((n = nextnode1(n)) != 0)
	    putchar(' ');
    }
}
#endif
