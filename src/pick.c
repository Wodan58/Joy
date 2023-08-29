/*
    module  : pick.c
    version : 1.2
    date    : 08/29/23
*/
#ifndef PICK_C
#define PICK_C

/**
OK 1216  pick  :  X Y Z 2  ->  X Y Z X
Pushes an extra copy of nth (e.g. 2) item X on top of the stack.
*/
void pick_(pEnv env)
{
    Index item;
    int i, size;

    FOURPARAMS("pick"); /* minimum of four: 0 pick == dup; 1 pick == over */
    INTEGER("pick");
    size = nodevalue(env->stck).num; /* pick up the number */
    POP(env->stck); /* remove top of stack */
    item = env->stck; /* can become 0 if the stack is too small */
    for (i = 0; i < size; i++) /* top of stack was popped */
	item = nextnode1(item);
    GNULLARY(nodetype(item), nodevalue(item));
}
#endif
