/*
    module  : __symtabindex.c
    version : 1.2
    date    : 04/11/22
*/
#ifndef __SYMTABINDEX_C
#define __SYMTABINDEX_C

/**
1060  __symtabindex  :  ->  I
Pushes current size of the symbol table.
*/
PUSH(__symtabindex_, INTEGER_NEWNODE, vec_size(env->symtab))



#endif
