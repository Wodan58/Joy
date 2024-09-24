/*
    module  : __symtabmax.c
    version : 1.3
    date    : 09/17/24
*/
#ifndef __SYMTABMAX_C
#define __SYMTABMAX_C

/**
Q0  OK  1050  __symtabmax  :  ->  I
Pushes value of maximum size of the symbol table.
*/
PUSH(__symtabmax_, INTEGER_NEWNODE, vec_max(env->symtab))



#endif
