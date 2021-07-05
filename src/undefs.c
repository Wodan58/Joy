/*
    module  : undefs.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef UNDEFS_C
#define UNDEFS_C

/**
1110  undefs  :  ->  [..]
Push a list of all undefined symbols in the current symbol table.
*/
PRIVATE void undefs_(pEnv env)
{
    pEntry i = symtabindex;
    Entry ent;

    env->dump1 = LIST_NEWNODE(NULL, env->dump1);
    while (i) {
        ent = vec_at(env->symtab, --i);
        if ((ent.name[0] != 0) && (ent.name[0] != '_') && (ent.u.body == NULL))
            DMP1 = STRING_NEWNODE(ent.name, DMP1);
    }
    NULLARY(LIST_NEWNODE, DMP1);
    POP(env->dump1);
}
#endif
