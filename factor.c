/* FILE: factor.c */
/*
 *  module  : factor.c
 *  version : 1.7
 *  date    : 04/13/22
 */
#include "globals.h"

/*
    readfactor - read a factor from srcfile and push it on the stack.
                 In case of LPAREN nothing gets pushed.
*/
PUBLIC void readfactor(pEnv env, int priv) /* read a JOY factor */
{
    long_t set = 0;

    switch (env->symb) {
    case ATOM:
        if (!priv) {
            lookup(env);
            if (env->location < env->firstlibra) {
                env->yylval.proc = vec_at(env->symtab, env->location).u.proc;
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
        while (getsym(env), env->symb != RBRACE)
            if ((env->symb != CHAR_ && env->symb != INTEGER_)
                || env->yylval.num < 0 || env->yylval.num >= SETSIZE)
                error(env, "small numeric expected in set");
            else
                set |= ((long_t)1 << env->yylval.num);
        if (!priv) {
            env->bucket.set = set;
            env->stck = newnode(env, SET_, env->bucket, env->stck);
        }
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

/*
    writefactor - print a factor in readable format.
*/
PUBLIC void writefactor(pEnv env, Index n, FILE *stm)
{
    int i;
    char *p;
    long_t set;

#if 0
/*
    This cannot happen. Factor has a small number of customers: writeterm,
    main, put, fput. They all check that the stack is not empty, so this code
    only serves as a reminder for future customers.
*/
    if (!n)
        execerror(env, "non-empty stack", "print");
#endif
    switch (opertype(nodetype(n))) {
    case USR_:
        fprintf(stm, "%s", vec_at(env->symtab, nodevalue(n).ent).name);
        return;
    case BOOLEAN_:
        fprintf(stm, "%s", nodevalue(n).num ? "true" : "false");
        return;
    case CHAR_:
        fprintf(stm, "'%c", (char)nodevalue(n).num);
        return;
    case INTEGER_:
        fprintf(stm, "%ld", (long)nodevalue(n).num); /* BIT_32 */
        return;
    case SET_:
        fputc('{', stm);
        for (i = 0, set = nodevalue(n).set; i < SETSIZE; i++)
            if (set & ((long_t)1 << i)) {
                fprintf(stm, "%d", i);
                set &= ~((long_t)1 << i);
                if (set)
                    fputc(' ', stm);
            }
        fputc('}', stm);
        return;
    case STRING_:
        fputc('"', stm);
        for (p = nodevalue(n).str; p && *p; p++) {
            if (*p == '"' || *p == '\\' || *p == '\n')
                fputc('\\', stm);
            fputc(*p == '\n' ? 'n' : *p, stm);
        }
        fputc('"', stm);
        return;
    case LIST_:
        fputc('[', stm);
        writeterm(env, nodevalue(n).lis, stm);
        fputc(']', stm);
        return;
    case FLOAT_:
        fprintf(stm, "%g", nodevalue(n).dbl);
        return;
    case FILE_:
        if (!nodevalue(n).fil)
            fprintf(stm, "file:NULL");
        else if (nodevalue(n).fil == stdin)
            fprintf(stm, "file:stdin");
        else if (nodevalue(n).fil == stdout)
            fprintf(stm, "file:stdout");
        else if (nodevalue(n).fil == stderr)
            fprintf(stm, "file:stderr");
        else
            fprintf(stm, "file:%p", nodevalue(n).fil);
        return;
    default:
        fprintf(stm, "%s", opername(nodetype(n)));
        return;
    }
}

/*
    writeterm - print the contents of a list in readable format.
*/
PUBLIC void writeterm(pEnv env, Index n, FILE *stm)
{
    while (n) {
        writefactor(env, n, stm);
        if ((n = nextnode1(n)) != 0)
            fputc(' ', stm);
    }
}
