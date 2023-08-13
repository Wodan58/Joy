/*
    module  : __symtabmax.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef __SYMTABMAX_C
#define __SYMTABMAX_C

/**
OK 1050  __symtabmax  :  ->  I
Pushes value of maximum size of the symbol table.
*/
PUSH(__symtabmax_, INTEGER_NEWNODE, vec_max(env->symtab))



#endif
