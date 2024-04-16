/*
    module  : help.h
    version : 1.5
    date    : 04/11/24
*/
#ifndef HELP_H
#define HELP_H

#define HELP(PROCEDURE, REL)						\
    void PROCEDURE(pEnv env)						\
    {									\
	int i = vec_size(env->symtab);					\
	int column = 0;							\
	int name_length;						\
	Entry ent;							\
	while (i) {							\
	    ent = vec_at(env->symtab, --i);				\
	    if (strchr("#0123456789_", ent.name[0]) REL 0) {		\
		name_length = strlen(ent.name) + 1;			\
		if (column + name_length > HELPLINEMAX) {		\
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
