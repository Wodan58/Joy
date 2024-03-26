/*
    module  : assign.c
    version : 1.1
    date    : 03/21/24
*/
#ifndef ASSIGN_C
#define ASSIGN_C

/**
IGNORE_POP  3190  assign  :  V [N]  ->
[IMPURE] Assigns value V to the variable with name N.
*/
void assign_(pEnv env)
{
    int index;
    Index lis, node;
    Entry ent;

    TWOPARAMS(__FILE__);		/* name and value */
    ONEQUOTE(__FILE__);			/* quotation on top */
    lis = nodevalue(env->stck).lis;	/* singleton list */
    CHECKEMPTYLIST(lis, __FILE__);	/* check non-empty list */
    USERDEF2(lis, __FILE__);		/* check user defined name */
    index = nodevalue(lis).ent;		/* index user defined name */
    ent = vec_at(env->symtab, index);	/* symbol table entry */
    lis = POP(env->stck);		/* value pointer */
    node = newnode(env, nodetype(lis), nodevalue(lis), 0);
    POP(env->stck);			/* bump stack again */
    ent.is_user = 1;			/* ensure again user defined */
    ent.u.body = node;			/* insert value in body */
    vec_at(env->symtab, index) = ent;	/* update symbol table */
}
#endif
