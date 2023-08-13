/*
    module  : stack.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef STACK_C
#define STACK_C

/**
OK 1040  stack  :  .. X Y Z  ->  .. X Y Z [Z Y X ..]
Pushes the stack as a list.
*/
PRIVATE void stack_(pEnv env) { NULLARY(LIST_NEWNODE, env->stck); }



#endif
