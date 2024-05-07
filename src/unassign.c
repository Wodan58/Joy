/*
    module  : unassign.c
    version : 1.1
    date    : 05/06/24
*/
#ifndef UNASSIGN_C
#define UNASSIGN_C

/**
IGNORE_POP  3235  unassign  :  [N]  ->
[IMPURE] Sets the body of the name N to uninitialized.
*/
void unassign_(pEnv env)
{
    Index lis;
    int index;
    Entry ent;

    ONEPARAM(__FILE__);			/* name */
    ONEQUOTE(__FILE__);			/* quotation on top */
    lis = nodevalue(env->stck).lis;	/* singleton list */
    CHECKEMPTYLIST(lis, __FILE__);	/* check non-empty list */
    USERDEF2(lis, __FILE__);		/* check user defined name */
    index = nodevalue(lis).ent;		/* index user defined name */
    ent = vec_at(env->symtab, index);	/* symbol table entry */
    POP(env->stck);			/* bump stack again */
    ent.is_user = 1;			/* ensure again user defined */
    ent.u.body = 0;			/* ensure empty body */
    vec_at(env->symtab, index) = ent;	/* update symbol table */
}
#endif
