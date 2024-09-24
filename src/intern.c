/*
    module  : intern.c
    version : 1.12
    date    : 09/17/24
*/
#ifndef INTERN_C
#define INTERN_C

/**
Q0  OK  2180  intern  :  "sym"  ->  sym
Pushes the item whose name is "sym".
*/
void intern_(pEnv env)
{
    int index;
    Entry ent;
    char *ptr, *str;

    ONEPARAM("intern");
    STRING("intern");
#ifdef NOBDW
    ptr = str = (char *)&nodevalue(env->stck);
#else
    ptr = str = nodevalue(env->stck).str;
#endif
    if (!strchr("\"#'().0123456789;[]{}", *ptr)) {
	if (*ptr == '-' && isdigit((int)ptr[1]))
	    ;
	else
	    for (++ptr; *ptr; ptr++)
		if (!isalnum((int)*ptr) && !strchr("-=_", *ptr))
		    break;
    }
    CHECKNAME(ptr, "intern");
    index = lookup(env, GC_strdup(str));
    ent = vec_at(env->symtab, index);
    if (ent.is_user)
	UNARY(USR_NEWNODE, index);
    else
	UNARY(ANON_FUNCT_NEWNODE, ent.u.proc);
}
#endif
