/*
    module  : __symtabmax.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef __SYMTABMAX_C
#define __SYMTABMAX_C

/**
1050  __symtabmax  :  ->  I
Pushes value of maximum size of the symbol table.
*/
PUSH(__symtabmax_, INTEGER_NEWNODE, vec_max(env->symtab))



#endif
