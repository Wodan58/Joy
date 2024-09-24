/*
    module  : __symtabindex.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef __SYMTABINDEX_C
#define __SYMTABINDEX_C

/**
Q0  OK  1060  __symtabindex  :  ->  I
Pushes current size of the symbol table.
*/
PUSH(__symtabindex_, INTEGER_NEWNODE, vec_size(env->symtab))



#endif
