/*
    module  : help.h
    version : 1.1
    date    : 05/21/21
*/
#ifndef HELP_H
#define HELP_H

#define HELP(PROCEDURE, REL)                                                   \
    PRIVATE void PROCEDURE(pEnv env)                                           \
    {                                                                          \
        pEntry i = symtabindex;                                                \
        int column = 0;                                                        \
        int name_length;                                                       \
        Entry ent;                                                             \
        while (i) {                                                            \
            ent = vec_at(env->symtab, --i);                                    \
            if (ent.name[0] REL '_' && !ent.is_local) {                        \
                name_length = strlen(ent.name) + 1;                            \
                if (column + name_length > 72) {                               \
                    printf("\n");                                              \
                    column = 0;                                                \
                }                                                              \
                printf("%s ", ent.name);                                       \
                column += name_length;                                         \
            }                                                                  \
        }                                                                      \
        printf("\n");                                                          \
    }
#endif
