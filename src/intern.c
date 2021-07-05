/*
    module  : intern.c
    version : 1.1
    date    : 05/21/21
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
    strncpy(ident, nodevalue(env->stck).str, ALEN);
    ident[ALEN - 1] = 0;
    p = 0;
    if (ident[0] == '-' || !strchr("(#)[]{}.;'\"0123456789", ident[0]))
        for (p = ident + 1; *p; p++)
            if (!isalnum((int)*p) && !strchr("=_-", *p))
                break;
    if (!p || *p)
        execerror(env, "valid name", ident);
    lookup(env, 0);
    if (location < firstlibra) {
        env->yylval.proc = vec_at(env->symtab, location).u.proc;
        GUNARY(location, env->yylval);
    } else
        UNARY(USR_NEWNODE, location);
}
#endif
