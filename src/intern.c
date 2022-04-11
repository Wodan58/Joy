/*
    module  : intern.c
    version : 1.2
    date    : 04/11/22
*/
#ifndef INTERN_C
#define INTERN_C

/**
2190  intern  :  "sym"  ->  sym
Pushes the item whose name is "sym".
*/
PRIVATE void intern_(pEnv env)
{
    char *p;

    ONEPARAM("intern");
    STRING("intern");
    strncpy(env->ident, nodevalue(env->stck).str, ALEN);
    env->ident[ALEN - 1] = 0;
    p = 0;
    if (*env->ident == '-' || !strchr("(#)[]{}.;'\"0123456789", *env->ident))
        for (p = env->ident + 1; *p; p++)
            if (!isalnum((int)*p) && !strchr("=_-", *p))
                break;
    if (!p || *p)
        execerror(env, "valid name", env->ident);
    lookup(env);
    if (env->location < env->firstlibra) {
        env->yylval.proc = vec_at(env->symtab, env->location).u.proc;
        GUNARY(env->location, env->yylval);
    } else
        UNARY(USR_NEWNODE, env->location);
}
#endif
