/*
    module  : name.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef NAME_C
#define NAME_C

/**
OK 2180  name  :  sym  ->  "sym"
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
