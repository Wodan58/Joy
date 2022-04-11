/* FILE: factor.c */
/*
 *  module  : factor.c
 *  version : 1.3
 *  date    : 04/11/22
 */
#include "globals.h"

/*
    readfactor - read a factor from srcfile and push it on the stack.
*/
PUBLIC void readfactor(pEnv env, int priv) /* read a JOY factor */
{
    Entry ent;
    long_t set = 0;
    pEntry mod_fields;

    switch (env->symb) {
    case ATOM:
        if (!priv) {
            lookup(env);
            if (env->location < env->firstlibra) {
                env->yylval.proc = vec_at(env->symtab, env->location).u.proc;
                env->stck = newnode(env, env->location, env->yylval, env->stck);
            } else
                env->stck = newnode(env, USR_, (Types)env->location, env->stck);
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
        if (!priv)
            env->stck = newnode(env, SET_, (Types)set, env->stck);
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
    writefactor - print a factor in readable format.
*/
PUBLIC void writefactor(pEnv env, Node *n, FILE *stm)
{
    int i;
    char *p;
    long_t set;

    if (!n)
        execerror(env, "non-empty stack", "print");
    switch (n->op) {
    case USR_:
        fprintf(stm, "%s", vec_at(env->symtab, n->u.ent).name);
        return;
    case BOOLEAN_:
        fprintf(stm, "%s", n->u.num ? "true" : "false");
        return;
    case CHAR_:
        fprintf(stm, "'%c", (char)n->u.num);
        return;
    case INTEGER_:
        fprintf(stm, "%ld", (long)n->u.num); /* BIT_32 */
        return;
    case SET_:
        fputc('{', stm);
        for (i = 0, set = n->u.set; i < SETSIZE; i++)
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
        for (p = n->u.str; p && *p; p++) {
            if (*p == '"' || *p == '\\' || *p == '\n')
                fputc('\\', stm);
            fputc(*p == '\n' ? 'n' : *p, stm);
        }
        fputc('"', stm);
        return;
    case LIST_:
        fputc('[', stm);
        writeterm(env, n->u.lis, stm);
        fputc(']', stm);
        return;
    case FLOAT_:
        fprintf(stm, "%g", n->u.dbl);
        return;
    case FILE_:
        if (!n->u.fil)
            fprintf(stm, "file:NULL");
        else if (n->u.fil == stdin)
            fprintf(stm, "file:stdin");
        else if (n->u.fil == stdout)
            fprintf(stm, "file:stdout");
        else if (n->u.fil == stderr)
            fprintf(stm, "file:stderr");
        else
            fprintf(stm, "file:%p", n->u.fil);
        return;
    default:
        fprintf(stm, "%s", opername(n->op));
        return;
    }
}
