/*
    module  : undefs.c
    version : 1.8
    date    : 09/26/24
*/
#ifndef UNDEFS_C
#define UNDEFS_C

/**
Q0  OK  1110  undefs  :  ->  [..]
[SYMBOLS] Push a list of all undefined symbols in the current symbol table.
*/
void undefs_(pEnv env)
{
    int i, j;
    Entry ent;
    Index *my_dump;

    NULLARY(LIST_NEWNODE, 0);
    my_dump = &nodevalue(env->stck).lis;
    for (i = 0, j = vec_size(env->symtab); i < j; i++) {
	ent = vec_at(env->symtab, i);
	if (ent.name[0] && ent.name[0] != '_' && !ent.u.body) {
	    *my_dump = STRING_NEWNODE(ent.name, 0);
	    my_dump = &nextnode1(*my_dump);
	}
    }
}
#endif
