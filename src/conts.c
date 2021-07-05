/*
    module  : conts.c
    version : 1.1
    date    : 05/21/21
*/
#ifndef CONTS_C
#define CONTS_C

/**
1080  conts  :  ->  [[P] [Q] ..]
Pushes current continuations. Buggy, do not use.
*/
PUSH(conts_, LIST_NEWNODE,
    LIST_NEWNODE(nextnode1(nodevalue(env->conts).lis), nextnode1(env->conts)))



#endif
