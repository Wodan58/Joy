/*
    module  : intern.c
    version : 1.3
    date    : 05/02/22
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
    char *ptr;

    ONEPARAM("intern");
    STRING("intern");
    strncpy(env->ident, nodevalue(env->stck).str, ALEN);
    env->ident[ALEN - 1] = 0;
    ptr = 0;
    if (*env->ident == '-' || !strchr("\"#'().0123456789;[]{}", *env->ident))
        for (ptr = env->ident + 1; *ptr; ptr++)
            if (!isalnum((int)*ptr) && !strchr("-=_", *ptr))
                break;
    CHECKNAME(ptr, "intern");
    lookup(env);
    ent = vec_at(env->symtab, env->location);
    if (!ent.is_user) {
        env->yylval.proc = ent.u.proc;
        GUNARY(env->location, env->yylval);
    } else
        UNARY(USR_NEWNODE, env->location);
}
#endif
