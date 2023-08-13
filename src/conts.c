/*
    module  : conts.c
    version : 1.2
    date    : 08/13/23
*/
#ifndef CONTS_C
#define CONTS_C

/**
OK 1080  conts  :  ->  [[P] [Q] ..]
Pushes current continuations. Buggy, do not use.
*/
PUSH(conts_, LIST_NEWNODE,
    LIST_NEWNODE(nextnode1(nodevalue(env->conts).lis), nextnode1(env->conts)))


#endif
