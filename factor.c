/* FILE: factor.c */
/*
 *  module  : factor.c
 *  version : 1.2
 *  date    : 05/23/21
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

PUBLIC void readfactor(pEnv env, int priv) /* read a JOY factor */
{
    long_t set = 0;
    pEntry mod_fields;
    Entry ent;

    switch (symb) {
    case ATOM:
        lookup(env, priv);
        while (location) {
            ent = vec_at(env->symtab, location);
            if (!ent.is_module)
                break;
            mod_fields = ent.u.module_fields;
            getsym(env);
            if (symb != PERIOD)
                error("period '.' expected after module name");
            else
                getsym(env);
            if (symb != ATOM) {
                error("atom expected as module field");
                return;
            }
            while (mod_fields) {
                ent = vec_at(env->symtab, mod_fields);
                if (!strcmp(ident, ent.name))
                    break;
                mod_fields = ent.next;
            }
            if (mod_fields == NULL) {
                error("no such field in module");
                abortexecution_(env);
            }
            location = mod_fields;
        }
        if (!priv) {
            if (location < firstlibra) {
                env->yylval.proc = vec_at(env->symtab, location).u.proc;
                env->stck = newnode(env, location, env->yylval, env->stck);
            } else
                env->stck = USR_NEWNODE(location, env->stck);
        }
        return;
    case BOOLEAN_:
    case CHAR_:
    case INTEGER_:
        if (!priv)
            env->stck = newnode(env, symb, env->yylval, env->stck);
        return;
    case STRING_:
        if (!priv)
            env->stck = newnode(env, symb, env->yylval, env->stck);
        return;
    case FLOAT_:
        if (!priv)
            env->stck = FLOAT_NEWNODE(env->yylval.dbl, env->stck);
        return;
    case LBRACE:
        while (getsym(env), symb != RBRACE)
            if (symb == CHAR_ || symb == INTEGER_)
                set |= ((long_t)1 << env->yylval.num);
            else
                error("numeric expected in set");
        if (!priv)
            env->stck = SET_NEWNODE(set, env->stck);
        return;
    case LBRACK:
        getsym(env);
        readterm(env, priv);
        if (symb != RBRACK)
            error("']' expected");
        return;
    case LPAREN:
        error("'(' not implemented");
        getsym(env);
        return;
    default:
        error("a factor cannot begin with this symbol");
    }
}

PUBLIC void my_writefactor(pEnv env, Node *n, FILE *stm)
{
    int i;
    char *p;
    long_t set;

    if (n == NULL)
        execerror(env, "non-empty stack", "print");
    switch (n->op) {
    case BOOLEAN_:
        fprintf(stm, "%s", n->u.num ? "true" : "false");
        return;
    case INTEGER_:
        fprintf(stm, "%ld", (long)n->u.num); /* BIT_32 */
        return;
    case FLOAT_:
        fprintf(stm, "%g", n->u.dbl);
        return;
    case SET_:
        set = n->u.set;
        fprintf(stm, "{");
        for (i = 0; i < SETSIZE; i++)
            if (set & ((long_t)1 << i)) {
                fprintf(stm, "%d", i);
                set &= ~((long_t)1 << i);
                if (set != 0)
                    fprintf(stm, " ");
            }
        fprintf(stm, "}");
        return;
    case CHAR_:
        fprintf(stm, "'%c", (char)n->u.num);
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
        fprintf(stm, "%s", "[");
        writeterm(env, n->u.lis, stm);
        fprintf(stm, "%s", "]");
        return;
    case USR_:
        fprintf(stm, "%s", vec_at(env->symtab, n->u.ent).name);
        return;
    case FILE_:
        if (n->u.fil == NULL)
            fprintf(stm, "file:NULL");
        else if (n->u.fil == stdin)
            fprintf(stm, "file:stdin");
        else if (n->u.fil == stdout)
            fprintf(stm, "file:stdout");
        else if (n->u.fil == stderr)
            fprintf(stm, "file:stderr");
        else
            fprintf(stm, "file:%p", (void *)n->u.fil);
        return;
    default:
        fprintf(stm, "%s", opername(n->op));
        return;
    }
}
