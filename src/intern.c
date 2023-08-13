/*
    module  : intern.c
    version : 1.6
    date    : 08/13/23
*/
#ifndef INTERN_C
#define INTERN_C

/**
OK 2190  intern  :  "sym"  ->  sym
Pushes the item whose name is "sym".
*/
PRIVATE void intern_(pEnv env)
{
    Entry ent;
    char ident[ALEN];
#ifdef CORRECT_INTERN_LOOKUP
    char *ptr;
#endif

    ONEPARAM("intern");
    STRING("intern");
    strncpy(ident, nodevalue(env->stck).str, ALEN);
    ident[ALEN - 1] = 0;
#ifdef CORRECT_INTERN_LOOKUP
    ptr = ident;
    if (!strchr("\"#'().0123456789;[]{}", *ptr)) {
        if (*ptr == '-' && isdigit((int)ptr[1]))
            ;
        else
            for (ptr++; *ptr; ptr++)
                if (!isalnum((int)*ptr) && !strchr("-=_", *ptr))
                    break;
    }
    CHECKNAME(ptr, "intern");
#endif
    env->yylval.str = GC_strdup(ident);
    lookup(env);
    ent = vec_at(env->symtab, env->location);
    if (!ent.is_user) {
        env->yylval.proc = ent.u.proc;
        GUNARY(env->location, env->yylval);
    } else
        UNARY(USR_NEWNODE, env->location);
}
#endif
