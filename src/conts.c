/*
    module  : conts.c
    version : 1.3
    date    : 09/17/24
*/
#ifndef CONTS_C
#define CONTS_C

/**
Q0  OK  1080  conts  :  ->  [[P] [Q] ..]
Pushes current continuations. Buggy, do not use.
*/
void conts_(pEnv env)
{
    Index temp;
    
    env->bucket.lis = nextnode1(nodevalue(env->conts).lis);
    temp = newnode(env, LIST_, env->bucket, nextnode1(env->conts));
    NULLARY(LIST_NEWNODE, temp);
/*
  PUSH(conts_, LIST_NEWNODE,
    LIST_NEWNODE(nextnode1(nodevalue(env->conts).lis), nextnode1(env->conts)))

  Code has unspecified behavior. Order of evaluation of function parameters or
  subexpressions is not defined, so if a value is used and modified in
  different places not separated by a sequence point constraining evaluation
  order, then the result of the expression is unspecified.
*/
}
#endif
