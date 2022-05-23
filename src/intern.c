/*
    module  : intern.c
    version : 1.4
    date    : 05/14/22
*/
#ifndef INTERN_C
#define INTERN_C

/**
2190  intern  :  "sym"  ->  sym
Pushes the item whose name is "sym".
*/
PRIVATE void intern_(pEnv env)
{
    Entry ent;
#ifdef CORRECT_INTERN_LOOKUP
    char *ptr;
#endif

    ONEPARAM("intern");
    STRING("intern");
    strncpy(env->ident, nodevalue(env->stck).str, ALEN);
    env->ident[ALEN - 1] = 0;
#ifdef CORRECT_INTERN_LOOKUP
    ptr = env->ident;
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
    lookup(env);
    ent = vec_at(env->symtab, env->location);
    if (!ent.is_user) {
        env->yylval.proc = ent.u.proc;
        GUNARY(env->location, env->yylval);
    } else
        UNARY(USR_NEWNODE, env->location);
}
#endif
