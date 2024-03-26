/*
 *  module  : undefs.c
 *  version : 1.1
 *  date    : 03/23/24
 */
#include "globals.h"

void hide_inner_modules(pEnv env, int flag)
{
    Node node;
    Entry ent;
    khiter_t key;
    char *name, *ptr;
    int i, leng, first = 0, last = 0;

    /*
     * There are two calls to this function. In the first call, the name of
     * the module is read and stored in name. In the second call, the name
     * is used when searching the symbol table.
     */
    if (flag) {
	node = vec_back(env->tokens);
	env->mod_name = node.u.str;
	return;
    }
    /*
     * At this point in the program, the symbol table is already filled with
     * the member functions and can be inspected right now.
     */
    name = env->mod_name;	/* name of module */
    leng = strlen(name);	/* length of name */
    /*
     * Look in the symbol table for the first and last mention of this name.
     * Stop looking at the first builtin.
     */
    for (i = vec_size(env->symtab) - 1; i >= 0; i--) {
	ent = vec_at(env->symtab, i);
	if (leng > (int)strlen(ent.name))	/* ent.name is too short */
	    continue;
	if (ent.name[leng] != '.')		/* ent.name is not a module */
	    continue;
	if (!strncmp(name, ent.name, leng)) {
	    if (!last)
		last = i;
	    first = i;				/* found one */
	}
	if (!ent.is_user)			/* builtins */
	    break;
    }
    /*
     * If there are other modules within this range, they should be
     * invalidated. This is accomplished by removing the module from the hash
     * table. The module is still present in the symbol table, but cannot be
     * found. Exactly what is needed.
     */
    for (i = first + 1; i < last; i++) {
	ent = vec_at(env->symtab, i);
	if (isdigit((int)ent.name[0]))		/* hidden names are ok */
	    continue;
	if ((ptr = strchr(ent.name, '.')) == 0)	/* ent.name is not a module */
	    continue;
	if (strncmp(name, ent.name, leng)) {	/* an inner module was found */
	    if ((key = kh_get(Symtab, env->hash, ent.name)) !=
			    kh_end(env->hash))
		kh_del(Symtab, env->hash, key);
	}
    }
}
