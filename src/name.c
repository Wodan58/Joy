/*
    module  : name.c
    version : 1.8
    date    : 09/17/24
*/
#ifndef NAME_C
#define NAME_C

/**
Q0  OK  2170  name  :  sym  ->  "sym"
For operators and combinators, the string "sym" is the name of item sym,
for literals sym the result string is its type.
*/
void name_(pEnv env)
{
    int op;
    char *str;

    ONEPARAM("name");
    if ((op = nodetype(env->stck)) == USR_)
	str = vec_at(env->symtab, nodevalue(env->stck).ent).name;
    else if (op == ANON_FUNCT_)
	str = nickname(operindex(env, nodevalue(env->stck).proc));
    else
	str = opername(op);
    UNARY(STRING_NEWNODE, str);
}
#endif
