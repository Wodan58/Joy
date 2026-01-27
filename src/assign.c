/*
    module  : assign.c
    version : 1.7
    date    : 01/17/26
*/
#ifndef ASSIGN_C
#define ASSIGN_C

/**
Q0  IGNORE_POP  3140  assign  :  V [N]  ->
[IMPURE] Assigns value V to the variable with name N.
*/
void assign_(pEnv env)
{
    int index;
    Entry ent;
    Index lis, node;

    TWOPARAMS("assign");		/* name and value */
    ONEQUOTE("assign");			/* quotation on top */
    lis = nodevalue(env->stck).lis;	/* singleton list */
    CHECKEMPTYLIST(lis, "assign");	/* check non-empty list */
    USERDEF2(lis, "assign");		/* check user defined name */
    index = nodevalue(lis).ent;		/* index user defined name */
    ent = vec_at(env->symtab, index);	/* symbol table entry */
    lis = POP(env->stck);		/* value pointer */
    node = newnode2(env, lis, 0);
    POP(env->stck);			/* bump stack again */
    ent.is_root = ent.is_user = 1;	/* ensure user defined and root */
    env->variable_busy = 1;		/* test whether this is the first */
    ent.u.body = node;			/* insert value in body */
    vec_at(env->symtab, index) = ent;	/* update symbol table */
}
#endif
