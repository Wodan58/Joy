/*
    module  : intern.c
    version : 1.7
    date    : 08/21/23
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
    char *ptr, *str;

    ONEPARAM("intern");
    STRING("intern");
    ptr = str = nodevalue(env->stck).str;
    if (!strchr("\"#'().0123456789;[]{}", *ptr)) {
        if (*ptr == '-' && isdigit((int)ptr[1]))
            ;
        else
            for (ptr++; *ptr; ptr++)
                if (!isalnum((int)*ptr) && !strchr("-=_", *ptr))
                    break;
    }
    CHECKNAME(ptr, "intern");
    env->yylval.str = str;
    lookup(env);
    ent = vec_at(env->symtab, env->location);
    if (!ent.is_user) {
        env->yylval.proc = ent.u.proc;
        GUNARY(env->location, env->yylval);
    } else
        UNARY(USR_NEWNODE, env->location);
}
#endif
