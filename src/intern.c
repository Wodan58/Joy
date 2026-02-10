/*
    module  : intern.c
    version : 1.15
    date    : 02/04/26
*/
#ifndef INTERN_C
#define INTERN_C

/**
Q0  OK  2180  intern  :  "sym"  ->  sym
[SYMBOLS] Pushes the item whose name is "sym".
*/
void intern_(pEnv env)
{
    int index;
    Entry ent;
    char *ptr, *str;

    ONEPARAM("intern");
    STRING("intern");
    ptr = str = GETSTRING(env->stck);
    if (isspace((int)*ptr))
	;
    else if (isdigit((int)*ptr) || (*ptr == '-' && isdigit((int)ptr[1])))
	;	/* number, not name */
    else
	for (++ptr; *ptr && !isspace((int)*ptr); ptr++)
	    if (strchr("\"#'().;[]{}", *ptr))	/* excluded from name */
		break;	/* LCOV_EXCL_LINE */
    CHECKNAME(ptr, "intern");
    index = lookup(env, str);
    ent = vec_at(env->symtab, index);
    if (ent.is_user)
	UNARY(USR_NEWNODE, index);
    else
	UNARY(ANON_FUNCT_NEWNODE, ent.u.proc);
}
#endif
