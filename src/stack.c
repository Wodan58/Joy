/*
    module  : stack.c
    version : 1.3
    date    : 03/21/24
*/
#ifndef STACK_C
#define STACK_C

/**
OK 1040  stack  :  .. X Y Z  ->  .. X Y Z [Z Y X ..]
Pushes the stack as a list.
*/
void stack_(pEnv env) { NULLARY(LIST_NEWNODE, env->stck); }



#endif
