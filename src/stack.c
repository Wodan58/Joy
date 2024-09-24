/*
    module  : stack.c
    version : 1.4
    date    : 09/17/24
*/
#ifndef STACK_C
#define STACK_C

/**
Q0  OK  1040  stack  :  .. X Y Z  ->  .. X Y Z [Z Y X ..]
Pushes the stack as a list.
*/
void stack_(pEnv env) { NULLARY(LIST_NEWNODE, env->stck); }



#endif
