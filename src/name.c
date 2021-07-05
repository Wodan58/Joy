/*
    module  : name.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef NAME_C
#define NAME_C

/**
2180  name  :  sym  ->  "sym"
For operators and combinators, the string "sym" is the name of item sym,
for literals sym the result string is its type.
*/
PRIVATE void name_(pEnv env)
{
    ONEPARAM("name");
    UNARY(STRING_NEWNODE, nodetype(env->stck) == USR_
            ? vec_at(env->symtab, nodevalue(env->stck).ent).name
            : opername(nodetype(env->stck)));
}
#endif
