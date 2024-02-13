/*
    module  : help.h
    version : 1.3
    date    : 01/22/24
*/
#ifndef HELP_H
#define HELP_H

#define HELP(PROCEDURE, REL)						\
    PRIVATE void PROCEDURE(pEnv env)					\
    {									\
	int i = vec_size(env->symtab);					\
	int column = 0;							\
	int name_length;						\
	Entry ent;							\
	if (env->ignore)						\
	    return;							\
	while (i) {							\
	    ent = vec_at(env->symtab, --i);				\
	    if (ent.name[0] REL '_' && !isdigit((int)ent.name[0])) {	\
		name_length = strlen(ent.name) + 1;			\
		if (column + name_length > 72) {			\
		    printf("\n");					\
		    column = 0;						\
		}							\
		printf("%s ", ent.name);				\
		column += name_length;					\
	    }								\
	}								\
	printf("\n");							\
    }
#endif
