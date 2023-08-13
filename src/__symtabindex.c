/*
    module  : __symtabindex.c
    version : 1.3
    date    : 08/13/23
*/
#ifndef __SYMTABINDEX_C
#define __SYMTABINDEX_C

/**
OK 1060  __symtabindex  :  ->  I
Pushes current size of the symbol table.
*/
PUSH(__symtabindex_, INTEGER_NEWNODE, vec_size(env->symtab))



#endif
