/*
    module  : body.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef BODY_C
#define BODY_C

/**
2200  body  :  U  ->  [P]
Quotation [P] is the body of user-defined symbol U.
*/
PRIVATE void body_(pEnv env)
{
    ONEPARAM("body");
    USERDEF("body");
    UNARY(LIST_NEWNODE, vec_at(env->symtab, nodevalue(env->stck).ent).u.body);
}
#endif
