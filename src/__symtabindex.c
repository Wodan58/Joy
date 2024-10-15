/*
    module  : __symtabindex.c
    version : 1.5
    date    : 09/26/24
*/
#ifndef __SYMTABINDEX_C
#define __SYMTABINDEX_C

/**
Q0  OK  1060  __symtabindex  :  ->  I
[SYMBOLS] Pushes current size of the symbol table.
*/
PUSH(__symtabindex_, INTEGER_NEWNODE, vec_size(env->symtab))



#endif
