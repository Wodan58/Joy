/*
    module  : body.c
    version : 1.3
    date    : 09/04/23
*/
#ifndef BODY_C
#define BODY_C

/**
OK 2190  body  :  U  ->  [P]
Quotation [P] is the body of user-defined symbol U.
*/
PRIVATE void body_(pEnv env)
{
    ONEPARAM("body");
    USERDEF("body");
    UNARY(LIST_NEWNODE, vec_at(env->symtab, nodevalue(env->stck).ent).u.body);
}
#endif
