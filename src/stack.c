/*
    module  : stack.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef STACK_C
#define STACK_C

/**
1040  stack  :  .. X Y Z  ->  .. X Y Z [Z Y X ..]
Pushes the stack as a list.
*/
PRIVATE void stack_(pEnv env) { NULLARY(LIST_NEWNODE, env->stck); }



#endif
