/*
    module  : unassign.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef UNASSIGN_C
#define UNASSIGN_C

/**
Q0  IGNORE_POP  3230  unassign  :  [N]  ->
[IMPURE] Sets the body of the name N to uninitialized.
*/
void unassign_(pEnv env)
{
    Index lis;
    int index;
    Entry ent;

    ONEPARAM(__FILE__);			/* name */
    ONEQUOTE("unassign");		/* quotation on top */
    lis = nodevalue(env->stck).lis;	/* singleton list */
    CHECKEMPTYLIST(lis, "unassign");	/* check non-empty list */
    USERDEF2(lis, "unassign");		/* check user defined name */
    index = nodevalue(lis).ent;		/* index user defined name */
    ent = vec_at(env->symtab, index);	/* symbol table entry */
    POP(env->stck);			/* bump stack again */
    ent.is_root = ent.is_user = 1;	/* ensure user defined and root */
    ent.u.body = 0;			/* ensure empty body */
    vec_at(env->symtab, index) = ent;	/* update symbol table */
}
#endif
